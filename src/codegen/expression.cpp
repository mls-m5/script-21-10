#include "expression.h"
#include "log.h"
#include "llvm/IR/Value.h"

llvm::Value *createLlvmBinaryExpression(Ast &op,
                                        llvm::Value *left,
                                        llvm::Value *right,
                                        CodegenContext &context) {
    if (op.token.content == "+") {
        return context.builder.CreateAdd(left, right, "tmpadd");
    }
    else if (op.token.content == "-") {
        return context.builder.CreateSub(left, right, "tmpsub");
    }
    //    else if (op.token.content == "/") {
    //        return context.builder.CreateDiv(left, right, "tmpsub");
    //    }
    else if (op.token.content == "*") {
        return context.builder.CreateMul(left, right, "tmpsub");
    }

    throw InternalError{op.token,
                        "could not create binary operator for " +
                            std::string{name(op.token.type)}};
}

llvm::Value *generateBinary(Ast &ast, CodegenContext &context) {
    scriptExpect(ast.size() == 3, ast.token, "binary ast is wrong size");
    scriptExpect(ast[1].type == Token::Operator,
                 ast[1].token,
                 "binary operation does not have operator in middle");

    auto l = generateExpression(ast[0], context);
    auto r = generateExpression(ast[2], context);
    return createLlvmBinaryExpression(ast[1], l, r, context);
}

llvm::Value *generateExpression(Ast &ast, CodegenContext &context) {
    switch (ast.type) {
    case Token::IntLiteral:
        return llvm::ConstantInt::get(
            context.context,
            llvm::APInt(32, std::stoll(std::string{ast.token.content}), true));

    case Token::BinaryOperation:
        return generateBinary(ast, context);

    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.token.type)}};
    }
}
