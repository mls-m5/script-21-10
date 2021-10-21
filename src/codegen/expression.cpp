#include "expression.h"
#include "log.h"
#include "llvm/IR/Value.h"

llvm::Value *generateExpression(Ast &ast, CodegenContext &context) {
    err("expression not implemented");

    return llvm::ConstantInt::get(context.context, llvm::APInt(32, 0, true));
}
