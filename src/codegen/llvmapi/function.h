#pragma once

#ifdef ENABLE_LLVM

#include "code/ast.h"
#include "codegen.h"
#include "context.h"
#include <llvm/IR/IRBuilder.h>

namespace llvmapi {

llvm::Function *generateFunctionPrototype(Ast &ast, CodegenContext &context);

llvm::Function *generateFunction(Ast &ast, CodegenContext &context);

llvm::Function *generateExternFunction(Ast &ast, CodegenContext &context);

} // namespace llvmapi

#endif
