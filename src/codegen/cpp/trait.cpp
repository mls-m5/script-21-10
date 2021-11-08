#include "trait.h"
#include "context.h"

namespace cpp {

Trait::Trait(const Ast &ast, Context &context, bool shouldExport)
    : shouldExport(shouldExport) {
    auto &nameAst = ast.get(Token::Name);

    name = nameAst.token.toString();

    auto &bodyAst = ast.get(Token::TraitBody);

    for (auto &memberAst : bodyAst) {
        if (memberAst.type != Token::FunctionPrototype &&
            memberAst.type != Token::TypedFunctionPrototype) {
            throw InternalError{memberAst.token,
                                "Expected function declaration"};
        }

        auto &nameAst = memberAst.getRecursive(Token::Name);
        auto pair = decltype(Trait::methods)::value_type{
            nameAst.token.toString(),
            generateFunctionPrototype(ast, context, shouldExport, false, true)};

        methods.emplace(std::move(pair));
    }
}

void generateTraitDeclaration(const Ast &ast,
                              Context &context,
                              bool shouldExport) {

    auto trait = Trait{ast, context, shouldExport};

    auto ss = std::ostringstream{};
    ss << "struct " << trait.name;
    auto oldInsertPoint =
        context.insertBlock({ss.str(), ast.token.loc, ast.token.buffer});

    for (auto &method : trait.methods) {
        context.insert(
            {method.second.methodSignature(context, trait.name, true) +
                 " = nullptr;",
             ast.token});
    }

    context.setInsertPoint(oldInsertPoint);
    context.insertBlock({";", ast.token});
}

} // namespace cpp
