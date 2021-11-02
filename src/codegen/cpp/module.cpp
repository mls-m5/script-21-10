#include "module.h"
#include "struct.h"

namespace cpp {

void generateExternDeclaration(Ast &ast, Context &context) {
    auto &declAst = ast.back();

    auto shouldDisableMangling =
        static_cast<bool>(ast.find(Token::String)); // Check for "C"

    if (declAst.type == Token::FunctionPrototype) {
        auto prototype =
            generateFunctionPrototype(declAst, context, shouldDisableMangling);
        context.insert({prototype.signature(context.moduleName) + ";",
                        ast.front().token.loc});
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
    if (ast.empty()) {
        return;
    }

    if (ast.front().type != Token::ModuleStatement) {
        throw InternalError{ast.token,
                            "Expected module statement at beginning of file" +
                                std::string{name(ast.type)}};
    }

    auto &moduleStatement = ast.front();
    auto &moduleName = moduleStatement.get(Token::Name);

    context.moduleName = moduleName.token.content;

    for (auto &child : ast) {
        generateRootNode(child, context);
    }
}

} // namespace cpp
