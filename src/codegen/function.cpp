#include "function.h"
#include "expression.h"

llvm::Function *generateFunctionPrototype(Ast &ast, CodegenContext &context) {
    auto argsSize = 1; // Todo: Read from ast

    //    std::vector<llvm::Type *> ArgTypes()

    throw std::runtime_error{"function prototype not implemented"};
}

llvm::Function *generateFunction(Ast &ast, CodegenContext &context) {
    auto name = ast.get(Token::Name).token.content;
    llvm::Function *function = context.module->getFunction(name);

    if (!function) {
        function = generateFunctionPrototype(ast, context);
    }

    if (!function) {
        return nullptr;
    }

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(context.context, "entry", function);
    context.builder.SetInsertPoint(block);

    context.scope.values.clear();

    for (auto &arg : function->args()) {
        context.scope.values[std::string{arg.getName()}] = &arg;
    }

    auto &body = ast.get(Token::Braces);

    if (auto *retVal = generateExpression(body, context)) {
        context.builder.CreateRet(retVal);

        // Todo: Verify function... ?

        return function;
    }

    function->eraseFromParent();
    return nullptr;
}
