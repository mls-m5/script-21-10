#pragma once
#include "code/ast.h"
#include "context.h"

namespace cpp {

Value generateExpression(Ast &ast, Context &context);
Value generateReturnExpression(const Value &, Context &context);

} // namespace cpp
