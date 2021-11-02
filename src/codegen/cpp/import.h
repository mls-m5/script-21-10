#pragma once

#include "code/ast.h"
#include "context.h"

namespace cpp {

void importModule(Ast &ast, Context &context, bool toGlobal);

} // namespace cpp
