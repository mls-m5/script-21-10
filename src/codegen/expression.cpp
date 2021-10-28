#include "expression.h"
#include "code/parser.h"
#include "log.h"
#include "struct.h"
#include "types.h"

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

    auto variable = context.scope.getVariable(std::string{ast.token.content});

    if (!variable) {
        throw InternalError{ast.token,
                            "cannot find variable " +
                                std::string{ast.token.content}};
    }

    auto load =
        context.builder.CreateLoad(variable->type,
                                   variable->alloca,
                                   false,
                                   "load" + std::string{ast.token.content});

    load->setAlignment(llvm::Align{variable->alloca->getAlignment()});

    return load;
}

llvm::Value *generateMemCpy(llvm::AllocaInst *dst,
                            llvm::AllocaInst *src,
                            size_t size,
                            CodegenContext &context) {

    return context.builder.CreateMemCpy(dst, {}, src, {}, size);
}

llvm::AllocaInst *generateVariableDeclaration(Ast &ast,
                                              CodegenContext &context) {

    auto nameAst = ast.findRecursive(Token::Name);
    if (!nameAst) {
        throw InternalError{
            ast.token,
            "Invalid variable declaration statement, no name found " +
                std::string{name(ast.type)}};
    }

    if (ast.back().type != Token::TypedVariable) {
        throw InternalError{ast.token,
                            "Untyped variables is not supported yet " +
                                std::string{name(ast.type)}};
    }

    // Todo: Support untyped variables
    auto &typeAst = ast.back().getRecursive(Token::TypeName);
    auto *type = getType(typeAst.token, context);

    auto function = context.builder.GetInsertBlock()->getParent();

    auto alloca = createEntryBlockAlloca(
        *function, std::string{nameAst->token.content}, type);

    context.scope.values[std::string{nameAst->token.content}] = {alloca, type};

    return alloca;
}

llvm::Value *generateAssignment(Ast &ast, CodegenContext &context) {
    auto &lhs = ast.front();

    if (lhs.type == Token::VariableDeclaration) {
        auto variable = generateVariableDeclaration(ast.front(), context);

        if (!variable) {
            throw InternalError{
                ast.token,
                "failed to create variable declaration. Value is not valid" +
                    std::string{name(ast.type)}};
        }

        auto pointeeType = variable->getAllocatedType();

        if (getStructFromType(pointeeType, context)) {
            // if (pointeeType->isStructTy()) {
            // Todo: Fix this in the future
            auto value = generateStructInitializer(ast.back(), context);
            auto structType = static_cast<llvm::StructType *>(pointeeType);

            auto size = context.module->getDataLayout()
                            .getStructLayout(structType)
                            ->getSizeInBytes();

            generateMemCpy(variable, value, size, context);
            return variable;
        }
        else {
            auto value = generateExpression(ast.back(), context);
            context.builder.CreateStore(value, variable);
            return value;
        }

        throw InternalError{ast.token,
                            "cannot handle assignment of pointer types" +
                                std::string{name(ast.type)}};
    }

    if (lhs.type != Token::Word) {
        throw InternalError{ast.token,
                            "can only assign to variables specified by name" +
                                std::string{name(ast.type)}};
    }

    auto alloca = context.scope.getVariable(std::string{lhs.token.content});

    if (!alloca) {
        throw InternalError{ast.token,
                            "cannot find variable " +
                                std::string{lhs.token.content}};
    }
    auto value = generateExpression(ast.back(), context);

    context.builder.CreateStore(value, alloca->alloca);

    return value;
}

llvm::Value *generateVariableLoad(llvm::Value *pointer,
                                  CodegenContext &context) {
    auto load = context.builder.CreateLoad(pointer->getType(), pointer, "load");

    return load;
}

} // namespace

llvm::Value *generateExpression(Ast &ast, CodegenContext &context) {
    throw InternalError{ast.token,
                        "Could not create expression of type " +
                            std::string{name(ast.type)}};
    switch (ast.type) {
    case Token::IntLiteral:
        return llvm::ConstantInt::get(
            context.context,
            llvm::APInt(32, std::stoll(std::string{ast.token.content}), true));

    case Token::BinaryOperation:
        return generateBinary(ast, context);

    case Token::FunctionCall:
        return generateFunctionCall(ast, context);
    case Token::Word:
        return generateVariableExpression(ast, context);
    case Token::VariableDeclaration:
        generateVariableDeclaration(ast, context);
        return nullptr;
    case Token::Assignment:
        return generateAssignment(ast, context);
    case Token::StructDeclaration:
        generateStructDeclaration(ast, context);
        return nullptr;
    case Token::ValueMemberAccessor:
        return generateVariableLoad(generateMemberAccessor(ast, context),
                                    context);
    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.type)}};
    }
}
