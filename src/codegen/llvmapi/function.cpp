#ifdef ENABLE_LLVM

#include "function.h"
#include "code/parser.h"
#include "expression.h"
#include "types.h"
#include <llvm/IR/Verifier.h>

using namespace llvmapi;

namespace {

llvm::Value *generateFunctionBody(Ast &ast, CodegenContext &context) {
    groupStandard(ast);

    llvm::Value *last = {};
    for (auto &child : ast) {
        last = generateExpression(child, context);
    }

    return last;
}

} // namespace

namespace llvmapi {

llvm::Function *generateFunctionPrototype(Ast &ast, CodegenContext &context) {
    auto astArgs = [&] {
        auto &astParentheses = ast.get(Token::FunctionArguments);
        groupStandard(astParentheses);
        return astParentheses.empty() ? std::vector<Ast *>{}
                                      : flattenList(astParentheses.front());
    }();

    auto argTypes = [&] {
        auto argTypes = std::vector<llvm::Type *>{};

        if (astArgs.empty()) {
            return argTypes;
        }

        for (auto *astArg : astArgs) {
            auto &typeAst = astArg->get(Token::TypeName);
            argTypes.push_back(getType(typeAst.token, context));
        }

        return argTypes;
    }();

    auto functionType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context.context), argTypes, false);

    auto &name = ast.get(Token::Name);

    auto func = llvm::Function::Create(functionType,
                                       llvm::Function::ExternalLinkage,
                                       name.token.content,
                                       context.module.get());

    context.scope().defineFunction(name.token, func);

    for (size_t i = 0; i < astArgs.size(); ++i) {
        auto *astArg = astArgs.at(i);
        auto arg = func->args().begin() + i;

        auto &name = astArg->get(Token::Name);

        arg->setName(name.token.content);
    }

    return func;
}

llvm::Function *generateFunction(Ast &ast, CodegenContext &context) {
    auto name = ast.get(Token::Name).token.content;
    llvm::Function *function = context.module->getFunction(name);

    auto push = PushValue{context.currentFunction, function};

    if (!function) {
        function = generateFunctionPrototype(ast, context);
    }

    if (!function) {
        return nullptr;
    }

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(context.context, "entry", function);
    context.builder.SetInsertPoint(block);

    //    auto scope = Scope{&context.scope};
    auto &scope = context.scope();

    for (auto &arg : function->args()) {
        auto type = arg.getType();
        auto alloca = createEntryBlockAlloca(*function, arg.getName(), type);

        // llvm will optimize away the store if it is possible to fit in
        // registers
        context.builder.CreateStore(&arg, alloca);
        scope.values[std::string{arg.getName()}] = {alloca, type};
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

llvm::Function *generateExternFunction(Ast &ast, CodegenContext &context) {

    auto &prototypeAst = ast.get(Token::FunctionPrototype);
    auto function = generateFunctionPrototype(prototypeAst, context);

    if (!function) {
        throw InternalError{ast.token,
                            "Failed to generate external expression"};
    }

    context.scope().defineFunction(prototypeAst.get(Token::Name).token,
                                   function);

    return function;
}

} // namespace llvmapi

#endif
