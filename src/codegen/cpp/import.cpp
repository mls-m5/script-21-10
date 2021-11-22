#include "import.h"
#include "code/parser.h"
#include "function.h"

using namespace cpp;

namespace {
void importStatement(const Ast &ast, Context &context) {
    if (ast.size() != 2) {
        throw InternalError{ast.token, "Malformed export statement"};
    }

    auto &child = ast.back();

    switch (child.type) {
    case Token::FunctionDeclaration:
    case Token::FunctionPrototype:
    case Token::TypedFunctionDeclaration:
    case Token::TypedFunctionPrototype: {
        auto prototype = generateFunctionPrototype(child, context, true);
        context.insert({prototype.signature(context) + ";", ast.front().token});
    }

    break;

    case Token::StructDeclaration:
        generateStructDeclaration(child, context);

    default:
        break;
    }
}
} // namespace

namespace cpp {

void importModule(const Ast &ast, Context &context, bool toGlobal) {
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

    context.moduleName(moduleName);

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

    context.moduleName("");
}

void handleImport(const Ast &importStatement, Context &context) {
    auto &moduleNameAst = importStatement.back();

    auto filename = context.fileLookup.find(moduleNameAst.token.toString());

    auto buffer = loadBufferFromFile(filename);

    if (!buffer) {
        throw InternalError{moduleNameAst.token,
                            "Could not find file for module " +
                                moduleNameAst.token.toString()};
    }

    auto moduleAst = parse(buffer);
    groupStandard(moduleAst, true);

    importModule(moduleAst, context, false);
}

} // namespace cpp
