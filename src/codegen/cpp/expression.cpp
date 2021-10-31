#include "expression.h"

using namespace cpp;

namespace {

Value generateIntLiteral(Ast &ast, Context &context) {
    auto id = context.generateId("c");
    auto text = "constexpr int " + id + " = " + ast.token.toString() + ";";
    context.insert({text, ast.token.loc});
    return {id};
}

Value generateBinaryOperation(Ast &ast, Context &context) {
    auto lhs = generateExpression(ast.front(), context);
    auto rhs = generateExpression(ast.back(), context);

    auto id = context.generateId("bin");
    auto text = "auto " + id + " = " + lhs.name + ast.at(1).token.toString() +
                rhs.name + ";";
    context.insert({text, ast.token.loc});

    return {id};
}

Value generateVariableExpression(Ast &ast, Context &context) {
    if (auto variable = context.getVariable(ast.token.toString())) {
        return {variable->name};
    }
    throw InternalError{
        ast.token, "Variable with name " + ast.token.toString() + " not found"};
}

Value generateVariableDeclaration(Ast &ast, Context &context) {
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
    auto variable = Variable{name};

    auto &typeAst = ast.back().getRecursive(Token::TypeName);
    auto type = context.getType(typeAst.token.content);

    if (type) {
        variable.ptr = type;
    }

    context.pushVariable(variable);

    context.insert(
        {typeAst.token.toString() + " " + name + ";", nameAst->token.loc});

    return {name};
}

Value generateStructInitializer(Ast &ast, Context &context) {
    if (!ast.empty()) {
        auto list = flattenList(ast.back().front());

        std::ostringstream ss;

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
        return {ss.str()};
    }

    return {"{}"};
}

Value generateAssignment(Ast &ast, Context &context) {
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
            {variable.name + " = " + value.name + ";", ast.front().token.loc});

        return variable;
    }

    if (lhs.type != Token::Word) {
        throw InternalError{ast.token,
                            "can only assign to variables specified by name" +
                                std::string{name(ast.type)}};
    }

    auto variable = context.getVariable(std::string{lhs.token.content});

    auto value = generateExpression(ast.back(), context);

    context.insert({variable->name + " = " + value.name + ";", lhs.token.loc});

    return value;
}

Value generateValueMemberAccessor(Ast &ast, Context &) {
    auto &lhs = ast.front();
    auto &rhs = ast.back();

    return {lhs.token.toString() + "." + rhs.token.toString()};
}

} // namespace

namespace cpp {

Value generateExpression(Ast &ast, Context &context) {
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

    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.type)}};
    }

    return {};
}

Value generateReturnExpression(const Value &value, Context &context) {
    context.insert({"return " + value.name + ";", Token::nloc});
    return {};
}

} // namespace cpp
