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
    auto text = lhs.type.type->name + " " + id + " = " + lhs.name +
                ast.at(1).token.toString() + rhs.name + ";";
    context.insert({text, ast.token});

    if (lhs.type.type != rhs.type.type) {
        throw InternalError{ast.token,
                            "could not operato on different types " +
                                lhs.type.type->name + " and " +
                                rhs.type.type->name};
    }

    return {id, lhs.type};
}

Value generateVariableExpression(const Ast &ast, Context &context) {
    if (auto variable = context.getVariable(ast.token.toString())) {
        return {variable->name, {variable->type}};
    }
    throw InternalError{
        ast.token, "Variable with name " + ast.token.toString() + " not found"};
}

Value generateVariableDeclaration(const Ast &ast, Context &context) {
    auto nameAst = ast.findRecursive(Token::Name);
    if (!nameAst) {
        throw InternalError{
            ast.token,
            "Invalid variable declaration statement, no name found " +
                std::string{name(ast.type)}};
    }

    if (ast.back().type != Token::TypedVariable) {
        throw InternalError{ast.token,
                            "Untyped variables is not supported yet " +
                                std::string{name(ast.type)}};
    }

    auto name = nameAst->token.toString();
    auto &typeAst = ast.back().getRecursive(Token::TypeName);
    auto type = context.getType(typeAst.token.content);

    auto variable = Variable{name, {type}};

    context.pushVariable(variable);

    context.insert(
        {typeAst.token.toString() + " " + name + ";", nameAst->token});

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

    return {"{}", {}};
}

Value generateAssignment(const Ast &ast, Context &context) {
    auto &lhs = ast.front();

    if (lhs.type == Token::VariableDeclaration) {
        auto variable = generateVariableDeclaration(ast.front(), context);

        //        if (!variable) {
        //            throw InternalError{
        //                ast.token,
        //                "failed to create variable declaration. Value is not
        //                valid" +
        //                    std::string{name(ast.type)}};
        //        }

        //        auto pointeeType = variable->getAllocatedType();

        //        if (getStructFromType(pointeeType, context.scope())) {
        //            // if (pointeeType->isStructTy()) {
        //            // Todo: Fix this in the future
        //            auto value = generateStructInitializer(ast.back(),
        //            context); auto structType = static_cast<llvm::StructType
        //            *>(pointeeType);

        //            auto size = context.module->getDataLayout()
        //                            .getStructLayout(structType)
        //                            ->getSizeInBytes();

        //            generateMemCpy(variable, value, size, context);
        //            return variable;
        //        }
        //        else {
        //            auto value = generateExpression(ast.back(), context);
        //            context.builder.CreateStore(value, variable);
        //            return value;
        //        }

        //        throw InternalError{ast.token,
        //                            "cannot handle assignment of pointer
        //                            types" +
        //                                std::string{name(ast.type)}};

        // Todo: Handle structs

        auto value = generateExpression(ast.back(), context);

        context.insert(
            {variable.name + " = " + value.name + ";", ast.front().token});

        return variable;
    }

    if (lhs.type != Token::Word) {
        throw InternalError{ast.token,
                            "can only assign to variables specified by name" +
                                std::string{name(ast.type)}};
    }

    auto variable = context.getVariable(std::string{lhs.token.content});

    auto value = generateExpression(ast.back(), context);

    context.insert({variable->name + " = " + value.name + ";", lhs.token});

    return value;
}

Value generateValueMemberAccessor(const Ast &ast, Context &context) {
    auto &lhsAst = ast.front();
    auto &rhsAst = ast.back();

    // Todo: Figure out this type

    auto lhs = generateExpression(lhsAst, context);
    auto rhs = rhsAst.token.toString();

    auto *s = lhs.type.type->structPtr;
    if (!s) {
        throw InternalError{lhsAst.token,
                            "not a struct" + lhsAst.token.toString()};
    }

    if (auto f =
            std::find_if(s->members.begin(),
                         s->members.end(),
                         [&rhs](auto &&member) { return member.name == rhs; });
        f != s->members.end()) {
        return {lhs.name + "." + rhs, f->type};
    }

    throw InternalError{rhsAst.token,
                        " could not find member " + rhs + " on struct " +
                            lhs.type.type->name};
}

Value generateStringLiteral(const Ast &ast, Context &context) {
    return {"str{" + ast.token.toString() + "}", {context.getType("str")}};
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
        return generateValueMemberAccessor(ast, context);
    case Token::String:
        return generateStringLiteral(ast, context);
    case Token::MemberName:
        return {ast.token.toString(), {}};

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
