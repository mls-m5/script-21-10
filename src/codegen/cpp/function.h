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

        SpecificType type;

        SpecificType getType(Context &context);

        // Noncaching version
        SpecificType getType(Context &context) const;
    };

    // @param moduleName is not used if shouldDisableMangling is true
    FunctionPrototype(const Ast &ast,
                      std::string_view moduleName,
                      Struct *self,
                      bool shouldExport,
                      bool shouldDisableMangling);

    FunctionPrototype(const FunctionPrototype &) = default;
    FunctionPrototype &operator=(const FunctionPrototype &) = default;
    FunctionPrototype(FunctionPrototype &&) = default;
    FunctionPrototype &operator=(FunctionPrototype &&) = default;

    std::string signature(Context &context);
    std::string methodSignature(Context &context, bool functionPointer = false);

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

    Struct *self();

private:
    SpecificType _returnType;
    Struct *_selfPtr;
};

FunctionPrototype generateFunctionPrototype(const Ast &ast,
                                            Context &context,
                                            bool shouldExport,
                                            bool shouldDisableMangling = false);

//! Returns the function prototype, you do not need no use it, but you could if
//! you need to know some information about it
FunctionPrototype generateFunctionDeclaration(
    const Ast &ast,
    Context &context,
    bool shouldExport,
    bool shouldDisableMangling = false);

//! @param ownerName: If called to a struct method, the named the variable with
//! the method called
Value generateFunctionCall(const Ast &ast, Context &context, Value owner = {});

} // namespace cpp
