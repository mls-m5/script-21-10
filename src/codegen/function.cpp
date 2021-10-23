#include "function.h"
#include "code/parser.h"
#include "expression.h"
#include "llvm/IR/Verifier.h"

namespace {

llvm::Value *generateFunctionBody(Ast &ast, CodegenContext &context) {
    ast = groupStandard(ast);

    // TODO: Does this only work for one expression?
    auto last = (llvm::Value *){};
    for (auto &child : ast) {
        last = generateExpression(child, context);
    }

    return last;
}

} // namespace

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

    if (auto *retVal = generateFunctionBody(body, context)) {
        context.builder.CreateRet(retVal);

        llvm::verifyFunction(*function);

        return function;
    }

    function->eraseFromParent();
    return nullptr;
}
