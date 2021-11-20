#pragma once
#include "code/ast.h"
#include "type.h"
#include <string>
#include <vector>

namespace cpp {

struct Context;
struct FunctionPrototype;
struct Trait;

struct Struct {
    struct Member {
        std::string name;
        SpecificType type;
        int pointerLevels = 0;
    };

    std::string name;

    std::vector<Member> members;
    std::vector<FunctionPrototype *> methods;
    std::vector<Trait *> traits;

    auto getMember(std::string_view name) -> Member *;
    auto getMethod(std::string_view name) -> FunctionPrototype *;

    bool hasTrait(std::string_view name);
    bool hasTrait(Trait *trait);
    void addTrait(Trait *trait);

    Struct() = default;

    Struct(const Ast &ast, Context &context);
};

void generateStructDeclaration(const Ast &ast, Context &context);

} // namespace cpp
