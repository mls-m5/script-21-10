#include "struct.h"
#include "code/parser.h"
#include "types.h"

llvm::Type *generateStructDeclaration(Ast &ast, CodegenContext &context) {
    auto s = Struct{};
    auto &structNameAst = ast.get(Token::Name);
    s.name = structNameAst.token;

    groupStandard(ast);

    for (auto &child : ast) {
        if (child.type == Token::TypedVariable) {
            auto &nameAst = child.get(Token::Name);
            auto &typeAst = child.getRecursive(Token::TypeName);
            auto type = getType(typeAst.token, context);
            auto newMember = Struct::StructMember{nameAst.token, type};
            s.members.push_back(newMember);
        }
    }

    auto types = std::vector<llvm::Type *>{};
    for (auto &m : s.members) {
        types.push_back(m.type);
    }

    auto type = llvm::StructType::create(context.context);
    type->setName(s.name.content);
    type->setBody(types);

    s.type = type;

    context.scope.setStruct(s.name.content, std::move(s));

    return type;
}