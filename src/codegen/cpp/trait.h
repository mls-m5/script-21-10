#pragma once
#include "code/ast.h"
#include "function.h"
#include <map>

namespace cpp {

struct Context;

struct Trait {

    std::map<std::string, FunctionPrototype> methods;

    Trait(const Ast &ast, Context &context, bool shouldExport);

    std::string name;

    bool shouldExport = false;
};

void generateTraitDeclaration(const Ast &ast,
                              Context &context,
                              bool shouldExport);

} // namespace cpp
