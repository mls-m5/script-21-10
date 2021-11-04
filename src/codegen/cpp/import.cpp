#include "import.h"
#include "function.h"

using namespace cpp;

namespace {
void importStatement(Ast &ast, Context &context) {
    if (ast.size() != 2) {
        throw InternalError{ast.token, "Malformed export statement"};
    }

    auto &child = ast.back();

    switch (child.type) {
    case Token::FunctionDeclaration:
    case Token::FunctionPrototype:
    case Token::TypedFunctionDeclaration:
    case Token::TypedFunctionPrototype:
        generateFunctionPrototype(child, context);

        break;

    case Token::StructDeclaration:
        generateStructDeclaration(child, context);

    default:
        break;
    }
}
} // namespace

namespace cpp {

void importModule(Ast &ast, Context &context, bool toGlobal) {
    auto moduleName = [&ast] {
        for (auto &child : ast) {
            switch (child.type) {
            case Token::ModuleStatement:
                return child.get(Token::Name).token.toString();
                break;
            default:
                break;
            }
        }
        return std::string{};
    }();

    // Todo: Handle global imports
    // Do so by adding references to the name-mangled functions
    // like:
    // othermodule_x();
    // static auto &x = othermodule_x;
    (void)toGlobal;

    context.moduleName = moduleName;

    for (auto &child : ast) {
        switch (child.type) {
        case Token::ModuleStatement:
            // Already handled
            break;

        case Token::ExportStatement:
            importStatement(child, context);
            break;
        default:
            // Just skip non-exported stuff
            break;
        }
    }

    context.moduleName = "";
}

} // namespace cpp
