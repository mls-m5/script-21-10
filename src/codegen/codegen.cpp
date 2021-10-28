#include "codegen.h"
#include "context.h"
#include "function.h"
#include "struct.h"

void generateModuleStatement(Ast &ast, CodegenContext &context) {}

void generateModuleCode(Ast &ast, CodegenContext &context) {
    std::string moduleId = "moduleid";

    for (auto &child : ast) {
        switch (child.type) {
        case Token::ModuleStatement:
            generateModuleStatement(ast, context);
            break;
        case Token::FunctionDeclaration:
            generateFunction(child, context);

            break;
        case Token::StructDeclaration:
            generateStructDeclaration(child, context);
            break;

        case Token::ExternStatement:
            generateExternFunction(child, context);
            break;
        default:
            throw SyntaxError{child.token,
                              "Could not parse global expression " +
                                  std::string{name(child.type)}};
        }
    }
}