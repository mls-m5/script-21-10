#include "import.h"
#include "function.h"
#include "struct.h"
#include <iostream>

using namespace llvmapi;

namespace {
void importStatement(Ast &ast, CodegenContext &context) {
    if (ast.size() != 2) {
        throw InternalError{ast.token, "Malformed export statement"};
    }

    auto &child = ast.back();

    switch (child.type) {
    case Token::FunctionDeclaration:
    case Token::FunctionPrototype:
        generateFunctionPrototype(child, context);

        break;

    case Token::StructDeclaration:
        generateStructDeclaration(child, context);

    default:
        break;
    }
}
} // namespace

namespace llvmapi {

void importModule(Ast &ast, CodegenContext &context, bool toGlobal) {
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

    auto oldScope = context.currentScope;
    context.currentScope = [&]() {
        if (toGlobal) {
            return oldScope;
        }
        else {
            auto &scope = context.importedModules[moduleName];
            scope.parent = context.currentScope;

            context.currentScope = &scope;

            scope.moduleName = moduleName;
            return &scope;
        }
    }();

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

    context.currentScope = oldScope;
}

} // namespace llvmapi