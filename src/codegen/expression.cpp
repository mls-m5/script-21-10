#include "expression.h"
#include "log.h"
#include "llvm/IR/Value.h"

llvm::Value *generateExpression(Ast &ast, CodegenContext &context) {
    switch (ast.type) {
    case Token::IntLiteral:
        return llvm::ConstantInt::get(
            context.context,
            llvm::APInt(32, std::stoll(std::string{ast.token.content}), true));

    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.token.type)}};
    }
}
