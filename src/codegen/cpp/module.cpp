#include "module.h"
#include "struct.h"

namespace cpp {

void generateExternDeclaration(Ast &ast, Context &context) {
    auto &declAst = ast.back();

    auto shouldDisableMangling =
        static_cast<bool>(ast.find(Token::String)); // Check for "C"

    if (declAst.type == Token::FunctionPrototype) {
        auto prototype =
            generateFunctionProototype(declAst, context, shouldDisableMangling);
        context.insert({prototype.signature() + ";", ast.front().token.loc});
    }
}

void generateRootNode(Ast &ast, Context &context) {
    switch (ast.type) {
    case Token::ModuleStatement:
        // Handled elsewhere
        break;
    case Token::FunctionDeclaration:
        generateFunctionDeclaration(ast, context);
        break;

    case Token::StructDeclaration:
        generateStructDeclaration(ast, context);
        break;

    case Token::ExternStatement:
        generateExternDeclaration(ast, context);
        break;

    default:
        throw InternalError{
            ast.token, "Invalid root element " + std::string{name(ast.type)}};
    }
}

void generateModule(Ast &ast, Context &context) {
    for (auto &child : ast) {
        generateRootNode(child, context);
    }
}

} // namespace cpp
