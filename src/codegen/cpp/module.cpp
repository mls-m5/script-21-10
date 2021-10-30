#include "module.h"

namespace cpp {

void generateRootNode(Ast &ast, Context &context) {
    switch (ast.type) {
    case Token::ModuleStatement:
        // Handled elsewhere
        break;
    case Token::FunctionDeclaration:
        generateFunctionDeclaration(ast, context);

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
