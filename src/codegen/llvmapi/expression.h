#pragma once
#include "code/ast.h"
#include "context.h"

namespace llvmapi {

llvm::Value *generateExpression(Ast &ast, CodegenContext &context);

} // namespace llvmapi
