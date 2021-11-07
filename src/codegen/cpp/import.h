#pragma once

#include "code/ast.h"
#include "context.h"

namespace cpp {

// Load the content of an ast
void importModule(const Ast &ast, Context &context, bool toGlobal);

// Handle a single import statement
void handleImport(const Ast &importStatement, Context &context);

} // namespace cpp
