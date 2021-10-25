#include "expression.h"
#include "code/parser.h"
#include "log.h"

namespace {

llvm::Value *createLlvmBinaryExpression(Ast &op,
                                        llvm::Value *left,
                                        llvm::Value *right,
                                        CodegenContext &context) {
    if (op.token.content == "+") {
        return context.builder.CreateAdd(left, right, "tmpadd");
    }
    else if (op.token.content == "-") {
        return context.builder.CreateSub(left, right, "tmpsub");
    }
    //    else if (op.token.content == "/") {
    //        return context.builder.CreateDiv(left, right, "tmpsub");
    //    }
    else if (op.token.content == "*") {
        return context.builder.CreateMul(left, right, "tmpsub");
    }

    throw InternalError{op.token,
                        "could not create binary operator for " +
                            std::string{name(op.token.type)}};
}

llvm::Value *generateBinary(Ast &ast, CodegenContext &context) {
    scriptExpect(ast.size() == 3, ast.token, "binary ast is wrong size");
    scriptExpect(ast[1].type == Token::Operator,
                 ast[1].token,
                 "binary operation does not have operator in middle");

    auto l = generateExpression(ast[0], context);
    auto r = generateExpression(ast[2], context);
    return createLlvmBinaryExpression(ast[1], l, r, context);
}

llvm::Value *generateFunctionCall(Ast &ast, CodegenContext &context) {
    // TODO:
    // https://github.com/llvm/llvm-project/blob/52f4922ebb7bfe5f9a6c32cf7d637b84e491526a/llvm/examples/Kaleidoscope/Chapter8/toy.cpp#L820
    std::vector<llvm::Value *> args;

    auto &astArgs = ast.get(Token::FunctionArguments);

    if (!astArgs.empty()) {
        groupStandard(astArgs);

        auto list = flattenList(astArgs.front());
        for (auto *arg : list) {
            args.push_back(generateExpression(*arg, context));
        }
    }

    auto &name = ast.get(Token::Name);
    if (auto f = context.scope.definedFunctions.find(
            std::string{name.token.content});
        f != context.scope.definedFunctions.end()) {
        auto function = f->second;

        if (function->arg_size() != args.size()) {
            throw InternalError{ast.token,
                                "trying to call function " +
                                    std::string{function->getName()} +
                                    " with " + std::to_string(astArgs.size()) +
                                    " arguments, but it only has " +
                                    std::to_string(function->arg_size())};
        }

        return context.builder.CreateCall(function, args, "calltmp");
    }
    else {
        throw InternalError{name.token,
                            "Could not find function " +
                                std::string{name.token.content}};
    }
}

llvm::Value *generateVariableExpression(Ast &ast, CodegenContext &context) {
    scriptExpect(ast.type == Token::Word,
                 ast.token,
                 "could not create variable from non word");

    return context.scope.getVariable(std::string{ast.token.content})->value;
}

} // namespace

llvm::Value *generateExpression(Ast &ast, CodegenContext &context) {
    switch (ast.type) {
    case Token::IntLiteral:
        return llvm::ConstantInt::get(
            context.context,
            llvm::APInt(64, std::stoll(std::string{ast.token.content}), true));

    case Token::BinaryOperation:
        return generateBinary(ast, context);

    case Token::FunctionCall:
        return generateFunctionCall(ast, context);
    case Token::Word:
        return generateVariableExpression(ast, context);

    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.type)}};
    }
}
