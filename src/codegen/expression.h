#pragma once
#include "ast.h"
#include "context.h"

llvm::Function *generateExpression(Ast &ast, CodegenContext &context);
