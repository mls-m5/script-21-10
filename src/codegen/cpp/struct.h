#pragma once
#include "code/ast.h"
#include "context.h"
#include <string>
#include <vector>

namespace cpp {

struct Struct {
    struct Member {
        std::string name;
        struct Type *type;
    };

    std::string name;

    std::vector<Member> members;

    Struct() = default;

    Struct(Ast &ast, Context &context);
};

void generateStructDeclaration(Ast &ast, Context &context);

} // namespace cpp
