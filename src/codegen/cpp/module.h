#pragma once

#include "code/ast.h"
#include "context.h"
#include <list>

namespace cpp {

void generateModule(const Ast &ast, Context &Context);

} // namespace cpp
