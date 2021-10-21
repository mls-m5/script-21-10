#include "codegen.h"
#include "context.h"
#include "function.h"

void generateModuleCode(Ast ast, CodegenContext &context) {
    std::string moduleId = "moduleid";

    for (auto &child : ast) {
        switch (child.type) {
        case Token::FunctionDeclaration: {
            auto ir = generateFunction(child, context);

        } break;
        default:
            break;
        }
    }
}
