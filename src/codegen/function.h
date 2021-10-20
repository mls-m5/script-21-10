#pragma once

#include "ast.h"
#include "codegen.h"
#include "context.h"
#include "llvm/IR/IRBuilder.h"

llvm::Function *generateFunctionPrototype(Ast &ast, CodegenContext &context);

llvm::Function *generateFunction(Ast &ast, CodegenContext &context);
