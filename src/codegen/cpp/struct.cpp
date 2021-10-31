#include "struct.h"

namespace cpp {

Struct::Struct(Ast &ast, Context &context) {
    auto &nameAst = ast.get(Token::Name);

    name = nameAst.token.toString();

    auto &bodyAst = ast.get(Token::StructBody);

    if (!bodyAst.empty()) {
        auto memberAsts = flattenList(bodyAst.front());

        for (auto memberAst : memberAsts) {
            auto &typeAst = memberAst->get(Token::TypeName);
            auto type = context.getType(typeAst.token.toString());

            auto &member = members.emplace_back();

            if (!type) {
                throw InternalError{typeAst.token,
                                    "could not find member of name " +
                                        typeAst.token.toString()};
            }
            member.type = type;

            member.name = memberAst->get(Token::Name).token.toString();
        }
    }
}

void generateStructDeclaration(Ast &ast, Context &context) {
    auto s = Struct(ast, context);

    auto lines = std::vector<std::string>{};

    lines.push_back("struct " + s.name + "{");
    for (auto &member : s.members) {
        lines.push_back("  " + member.type->name + " " + member.name + ";");
    }

    lines.push_back("};");

    for (auto &line : lines) {
        context.insert({std::move(line), ast.front().token.loc});
    }

    context.setStruct(s);
}

} // namespace cpp
