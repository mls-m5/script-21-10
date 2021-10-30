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

} // namespace

namespace cpp {

Value generateExpression(Ast &ast, Context &context) {
    switch (ast.type) {
    case Token::IntLiteral:
        return generateIntLiteral(ast, context);
    case Token::BinaryOperation:
        return generateBinaryOperation(ast, context);
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
