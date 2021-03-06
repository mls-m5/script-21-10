#pragma once
#include "code/ast.h"
#include "context.h"

namespace cpp {

Value generateExpression(const Ast &ast, Context &context);
Value generateReturnExpression(const Value &,
                               Context &context,
                               const Token &copyLocation);

} // namespace cpp
