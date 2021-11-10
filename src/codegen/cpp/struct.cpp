#include "struct.h"
#include "context.h"
#include "log.h"

namespace cpp {

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
