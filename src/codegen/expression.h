#pragma once
#include "code/ast.h"
#include "context.h"

llvm::Function *generateExpression(Ast &ast, CodegenContext &context);
