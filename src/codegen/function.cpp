#include "function.h"
#include "code/parser.h"
#include "expression.h"
#include "llvm/IR/Verifier.h"

namespace {

llvm::Value *generateFunctionBody(Ast &ast, CodegenContext &context) {
    groupStandard(ast);

    // TODO: Does this only work for one expression?
    auto last = (llvm::Value *){};
    for (auto &child : ast) {
        last = generateExpression(child, context);
    }

    return last;
}

llvm::Type *getType(const Token &typeName, CodegenContext &context) {
    if (typeName.content == "int") {
        return llvm::Type::getInt64Ty(context.context);
    }

    throw InternalError{typeName, "not recognized type"};
}

} // namespace

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
            // TODO: Implement argument types
            //            argTypes.push_back(getType(astArg->back().token,
            //            context));
            argTypes.push_back(llvm::Type::getInt64Ty(context.context));
        }

        return argTypes;
    }();

    auto functionType = llvm::FunctionType::get(
        llvm::Type::getInt64Ty(context.context), argTypes, false);

    auto &name = ast.get(Token::Name);

    auto func = llvm::Function::Create(functionType,
                                       llvm::Function::ExternalLinkage,
                                       name.token.content,
                                       context.module.get());

    context.scope.defineFunction(name.token, func);

    for (size_t i = 0; i < astArgs.size(); ++i) {
        auto *astArg = astArgs.at(i);
        auto arg = func->args().begin() + i;

        arg->setName(
            astArg->token.content); // Todo: Fix when implementing types
    }

    return func;
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

    //    auto scope = Scope{&context.scope};
    auto &scope = context.scope;

    for (auto &arg : function->args()) {
        // TODO: implement types
        auto type = llvm::Type::getInt64Ty(context.context);
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
