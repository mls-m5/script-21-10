#include "codegen.h"
#include "context.h"
#include "function.h"

std::unique_ptr<llvm::Module> generateModuleCode(Ast ast) {
    std::string moduleId = "moduleid";

    auto context = std::make_unique<CodegenContext>();
    auto module = std::make_unique<llvm::Module>(moduleId, *context->context);

    for (auto &child : ast) {
        switch (child.type) {
        case Token::FunctionDeclaration: {
            auto ir = generateFunction(ast, *context);
        } break;
        default:
            break;
        }
    }

    throw std::runtime_error{"generateModuleCode not fully implemented"};
}
