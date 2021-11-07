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
        std::string _typeName;
        int pointer;

        SpecificType type = {};

        SpecificType getType(Context &context);

        // Noncaching version
        SpecificType getType(Context &context) const;
    };

    // @param moduleName is not used if shouldDisableMangling is true
    FunctionPrototype(const Ast &ast,
                      std::string_view moduleName,
                      bool shouldExport,
                      bool shouldDisableMangling);

    std::string signature(Context &context);

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

    bool shouldExport = false;

    SpecificType returnType(Context &context);

private:
    SpecificType _returnType;
};

FunctionPrototype generateFunctionPrototype(const Ast &ast,
                                            Context &context,
                                            bool shouldExport,
                                            bool shouldDisableMangling = false);

void generateFunctionDeclaration(const Ast &ast,
                                 Context &context,
                                 bool shouldExport);

Value generateFunctionCall(const Ast &ast, Context &context);

} // namespace cpp
