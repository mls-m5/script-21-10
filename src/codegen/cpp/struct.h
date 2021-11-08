#pragma once
#include "code/ast.h"
#include "type.h"
#include <string>
#include <vector>

namespace cpp {

struct Context;

struct Struct {
    struct Member {
        std::string name;
        SpecificType type;
        int pointerLevels = 0;
    };

    std::string name;

    std::vector<Member> members;

    Struct() = default;

    Struct(const Ast &ast, Context &context);
};

void generateStructDeclaration(const Ast &ast, Context &context);

} // namespace cpp
