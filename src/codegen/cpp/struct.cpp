#include "struct.h"
#include "context.h"
#include "log.h"
#include <algorithm>

namespace cpp {

Struct::Member *Struct::getMember(std::string_view name) {
    if (auto f = std::find_if(members.begin(),
                              members.end(),
                              [&name](auto &m) { return m.name == name; });
        f != members.end()) {
        return f.base();
    }

    return nullptr;
}

FunctionPrototype *Struct::getMethod(std::string_view name) {
    if (auto f = std::find_if(methods.begin(),
                              methods.end(),
                              [&name](auto &m) { return m->name == name; });
        f != methods.end()) {
        return *f;
    }

    return nullptr;
}

bool Struct::hasTrait(std::string_view name) {
    if (auto f = std::find_if(traits.begin(),
                              traits.end(),
                              [&name](auto &m) { return m->name == name; });
        f != traits.end()) {
        return true;
    }

    return false;
}

void Struct::addTrait(Trait *trait) {
    traits.push_back(trait);
}

Struct::Struct(const Ast &ast, Context &context) {
    auto &nameAst = ast.get(Token::Name);

    name = nameAst.token.toString();

    auto &bodyAst = ast.get(Token::StructBody);

    for (auto &memberAst : bodyAst) {
        auto &typeAst = memberAst.getRecursive(Token::TypeName);
        auto type = context.getType(typeAst.token.toString());

        auto &member = members.emplace_back();

        if (!type) {
            throw InternalError{typeAst.token,
                                "could not find member type of name " +
                                    typeAst.token.toString()};
        }
        member.type = {type};

        member.name = memberAst.getRecursive(Token::Name).token.toString();
        member.pointerLevels = memberAst.type == Token::PointerTypedVariable;
    }
}

void generateStructDeclaration(const Ast &ast, Context &context) {
    auto s = Struct(ast, context);
    context.setStruct(s);
    if (s.name == "str") {
        dlog("skipping output of built in struct str");
        return;
    }

    auto oldInsertPoint = context.insertBlock({"struct " + s.name, ast.token});

    for (auto &member : s.members) {
        context.insert(
            {member.type.type->name + " " + member.name + ";", ast.token});
    }

    context.setInsertPoint(oldInsertPoint);

    context.insert({";", ast.token});
}

} // namespace cpp
