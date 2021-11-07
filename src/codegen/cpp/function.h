#pragma once

#include "code/ast.h"
#include "value.h"
#include <string>
#include <vector>

namespace cpp {

struct Context;

struct FunctionPrototype {
    struct Arg {
        std::string name;
        std::string type; // Todo: Handle other types
    };

    // @param moduleName is not used if shouldDisableMangling is true
    FunctionPrototype(const Ast &ast,
                      std::string_view moduleName,
                      bool shouldDisableMangling);

    std::string signature();

    std::string mangledName();

    //! When a function is part of the current translation module or if a module
    //! is imported without module name
    std::string localName();

    bool shouldDisableMangling = false;
    std::string name;
    TokenLocation location;
    std::vector<Arg> args;
    std::string moduleName;

    std::string returnTypeName = "void";
};

FunctionPrototype generateFunctionPrototype(const Ast &ast,
                                            Context &context,
                                            bool shouldDisableMangling = false);

void generateFunctionDeclaration(const Ast &ast, Context &context);

Value generateFunctionCall(const Ast &ast, Context &context);

} // namespace cpp
