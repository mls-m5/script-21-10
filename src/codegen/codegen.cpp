#include "codegen.h"
#include "context.h"
#include "function.h"

Ast generateModuleCode(Ast ast, CodegenContext &context) {
    std::string moduleId = "moduleid";

    for (auto &child : ast) {
        switch (child.type) {
        case Token::FunctionDeclaration: {
            generateFunction(child, context);

        } break;
        default:
            break;
        }
    }

    return ast;
}
