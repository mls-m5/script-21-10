#include "function.h"
#include "expression.h"

llvm::Function *generateFunctionPrototype(Ast &ast, CodegenContext &context) {
    auto argsSize = 0; // Todo: Read from ast
    auto argTypes = std::vector<llvm::Type *>(argsSize);

    auto functionType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(context.context), false);

    auto func = llvm::Function::Create(functionType,
                                       llvm::Function::ExternalLinkage,
                                       ast.get(Token::Name).token.content,
                                       context.module.get());

    return func;
    //    throw std::runtime_error{"function prototype not implemented"};
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

    auto &body = ast.get(Token::FunctionBody);

    if (auto *retVal = generateExpression(body, context)) {
        context.builder.CreateRet(retVal);

        // Todo: Verify function... ?

        return function;
    }

    function->eraseFromParent();
    return nullptr;
}
