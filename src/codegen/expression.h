#pragma once
#include "code/ast.h"
#include "context.h"

llvm::Value *generateExpression(Ast &ast, CodegenContext &context);
