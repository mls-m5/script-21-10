#include "expression.h"
#include <algorithm>

using namespace cpp;

namespace {

Value generateIntLiteral(const Ast &ast, Context &context) {
    auto id = context.generateId("c");
    auto text = "constexpr int " + id + " = " + ast.token.toString() + ";";
    context.insert({text, ast.token});
    return {id, {context.getType("int")}};
}

Value generateBinaryOperation(const Ast &ast, Context &context) {
    auto lhs = generateExpression(ast.front(), context);
    auto rhs = generateExpression(ast.back(), context);

    auto id = context.generateId("bin");
    auto text = lhs.type.type()->name + " " + id + " = " + lhs.name +
                ast.at(1).token.toString() + rhs.name + ";";
    context.insert({text, ast.token});

    if (lhs.type.type() != rhs.type.type()) {
        throw InternalError{ast.token,
                            "could not operato on different types " +
                                lhs.type.type()->name + " and " +
                                rhs.type.type()->name};
    }

    return {id, lhs.type};
}

Value generateVariableExpression(const Ast &ast, Context &context) {
    auto name = ast.token.toString();
    if (auto variable = context.getVariable(name)) {
        return {variable->name, {variable->type}};
    }
    else if (auto self = context.selfStruct()) {
        if (auto member = self->getMember(name)) {
            return {"self." + name, member->type};
        }
    }
    throw InternalError{
        ast.token, "Variable with name " + ast.token.toString() + " not found"};
}

Value generateVariableDeclaration(const Ast &ast,
                                  Context &context,
                                  SpecificType type) {
    auto nameAst = ast.findRecursive(Token::Name);
    if (!nameAst) {
        throw InternalError{
            ast.token,
            "Invalid variable declaration statement, no name found " +
                std::string{name(ast.type)}};
    }

    auto name = nameAst->token.toString();

    if (!type.type()) {
        auto &typeAst = ast.back().getRecursive(Token::TypeName);
        //        type.type() = context.getType(typeAst.token.content);
        type = {context.getType(typeAst.token.content)};
    }

    auto variable = Variable{name, {type}};

    context.pushVariable(variable);

    context.insert({type.toString() + " " + name + ";", nameAst->token});

    return {name, {type}};
}

Value generateStructInitializer(const Ast &ast, Context &context) {
    if (!ast.empty()) {
        auto list = flattenList(ast.back().front());

        std::ostringstream ss;

        auto structName = ast.front().token.toString();

        auto *type = context.getType(structName);

        if (!type) {
            throw InternalError{ast.front().token,
                                "could not find type with name " + structName};
        }

        if (!type->structPtr) {
            throw InternalError{ast.front().token,
                                structName + " is not a struct"};
        }

        ss << ast.front().token.toString() << "{ ";
        for (auto &member : list) {
            if (member->type == Token::Assignment) {
                ss << "." << member->front().token.toString() << "="
                   << generateExpression(member->back(), context).name << ", ";
            }
            else {
                ss << generateExpression(*member, context).name << ", ";
            }
        }
        ss << " }";
        return {ss.str(), {type}};
    }

    return {"{}", {context.getType("void")}};
}

Value generateAssignment(const Ast &ast, Context &context) {
    auto &lhsAst = ast.front();

    if (lhsAst.type == Token::VariableDeclaration) {
        auto value = generateExpression(ast.back(), context);

        auto variable =
            generateVariableDeclaration(ast.front(), context, value.type);

        context.insert(
            {variable.name + " = " + value.name + ";", ast.front().token});

        return variable;
    }

    auto lhs = generateExpression(lhsAst, context);

    auto value = generateExpression(ast.back(), context);

    if (lhs.type != value.type) {
        throw InternalError{lhsAst.token,
                            "cannot assign variable of type " +
                                lhs.type.toString() + " with value of type " +
                                value.type.toString()};
    }

    context.insert({lhs.name + " = " + value.name + ";", lhsAst.token});

    return value;
}

Value generateMemberAccessor(const Ast &ast, Context &context) {
    auto &lhsAst = ast.front();
    auto &rhsAst = ast.back();

    auto lhs = generateExpression(lhsAst, context);
    auto rhs = rhsAst.token.toString();

    auto *s = lhs.type.type()->structPtr;
    if (!s) {
        throw InternalError{lhsAst.token,
                            "not a struct" + lhsAst.token.toString()};
    }

    if (auto f = s->getMember(rhs)) {
        auto op = ast.type == Token::PointerMemberAccessor ? "->" : ".";
        return {lhs.name + op + rhs, f->type};
    }

    throw InternalError{rhsAst.token,
                        " could not find member " + rhs + " on struct " +
                            lhs.type.type()->name};
}

Value generateStringLiteral(const Ast &ast, Context &context) {
    return {"str{" + ast.token.toString() + "}", {context.getType("str")}};
}

Value generateReferencingStatement(const Ast &ast, Context &context) {
    auto value = generateExpression(ast.back(), context);
    auto type = value.type;
    return {"&" + value.name,
            {type.type(), type.pointerDepth() + 1, type.isReference()}};
}

Value generateCastStatement(const Ast &ast, Context &context) {
    auto value = generateExpression(ast.front(), context);

    if (ast.back().type != Token::TypeName) {
        throw InternalError{ast.token, "Expected word describing type"};
    }

    auto type = context.getType(ast.back());

    if (!type.type()) {
        throw InternalError{ast.back().token,
                            "Could not find type " +
                                ast.back().token.toString()};
    }

    auto id = context.generateId("cast");
    if (auto trait = type.type()->traitPtr) {
        auto fromStruct = value.type.type()->structPtr;
        if (!fromStruct->hasTrait(trait)) {
            throw InternalError{ast.token,
                                " Type " + value.type.toString() +
                                    " does not implement trait " + trait->name +
                                    ". Did you miss to add 'trait " +
                                    trait->name + "' to your impl statement?"};
        }

        context.insert(
            {"auto " + id + " = " + type.toString() + "{" + value.name + ", &" +
                 trait->vtableNameForStruct(*value.type.type()->structPtr) +
                 "};",
             ast.token});

        return {id, type};
    }
    else {

        context.insert({"auto " + id + " = static_cast<" + type.toString() +
                            ">(" + value.name + ");",
                        ast.token});

        return {id, type};
    }
}

Value generateParenthesesStatement(const Ast &ast, Context &context) {
    if (ast.empty()) {
        return {"()", {context.getType("void")}};
    }
    if (ast.size() > 1) {
        throw InternalError{ast.at(1).token, "Unexpected token"};
    }

    auto value = generateExpression(ast.front(), context);
    return {"(" + value.name + ")", value.type};
}

} // namespace

namespace cpp {

Value generateExpression(const Ast &ast, Context &context) {
    switch (ast.type) {
    case Token::IntLiteral:
        return generateIntLiteral(ast, context);
    case Token::BinaryOperation:
        return generateBinaryOperation(ast, context);
    case Token::Word:
        return generateVariableExpression(ast, context);
    case Token::FunctionCall:
        return generateFunctionCall(ast, context);
    case Token::Assignment:
        return generateAssignment(ast, context);
    case Token::StructInitializer:
        return generateStructInitializer(ast, context);
    case Token::ValueMemberAccessor:
        return generateMemberAccessor(ast, context);
    case Token::PointerMemberAccessor:
        return generateMemberAccessor(ast, context);
    case Token::String:
        return generateStringLiteral(ast, context);
    case Token::MemberName:
        return {ast.token.toString(), {}};
    case Token::ReferencingStatement:
        return generateReferencingStatement(ast, context);
    case Token::AsStatement:
        return generateCastStatement(ast, context);
    case Token::Parentheses:
        return generateParenthesesStatement(ast, context);

    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.type)}};
    }

    return {};
}

Value generateReturnExpression(const Value &value,
                               Context &context,
                               const Token &copyLocationFrom) {
    context.insert({"return " + value.name + ";", copyLocationFrom});
    return {};
}

} // namespace cpp
