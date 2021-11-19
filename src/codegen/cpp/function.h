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
                      Struct *self,
                      bool shouldExport,
                      bool shouldDisableMangling,
                      bool isMethod);

    std::string signature(Context &context);
    std::string methodSignature(Context &context,
                                std::string_view parentName,
                                bool functionPointer = false);
    //    std::string lambdaSignature(Context &context);

    std::string mangledName(std::string_view parentName = {});

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
    bool isMethod = false;

    SpecificType returnType(Context &context);

private:
    SpecificType _returnType;
    Struct *_selfPtr;
};

FunctionPrototype generateFunctionPrototype(const Ast &ast,
                                            Context &context,
                                            bool shouldExport,
                                            bool shouldDisableMangling = false,
                                            bool isMethod = false);

//! Returns the function prototype, you do not need no use it, but you could if
//! you need to know some information about it
FunctionPrototype generateFunctionDeclaration(
    const Ast &ast,
    Context &context,
    bool shouldExport,
    bool shouldDisableMangling = false,
    bool isMethod = false);

Value generateFunctionCall(const Ast &ast, Context &context);

} // namespace cpp
