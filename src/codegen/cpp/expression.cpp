#include "expression.h"

using namespace cpp;

namespace {

Value generateIntLiteral(Ast &ast, Context &context) {
    auto id = context.generateId("c");
    auto text = "constexpr int " + id + " = " + ast.token.toString() + ";";
    context.insert({text, ast.token.loc});
    return {id};
}

} // namespace

namespace cpp {

Value generateExpression(Ast &ast, Context &context) {
    switch (ast.type) {
    case Token::IntLiteral:
        return generateIntLiteral(ast, context);
    default:
        break;
    }

    return {};
}

Value generateReturnExpression(const Value &value, Context &context) {
    context.insert({"return " + value.name + ";", Token::nloc});
    return {};
}

} // namespace cpp
