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

    FunctionPrototype(Ast &ast);

    std::string name;
    TokenLocation location;
    std::vector<Arg> args;
};

FunctionPrototype generateFunctionProototype(Ast &ast, Context &context);

void generateFunctionDeclaration(Ast &ast, Context &context);

Value generateFunctionCall(Ast &ast, Context &context);

} // namespace cpp
