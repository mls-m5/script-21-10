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

    std::string mangledName();
    std::string vtableName();
    std::string vtableNameForStruct(Struct &);

    bool shouldExport = false;
};

void generateTraitDeclaration(const Ast &ast,
                              Context &context,
                              bool shouldExport);

void generateImplDeclaration(const Ast &ast,
                             Context &context,
                             bool shouldExport);

} // namespace cpp
