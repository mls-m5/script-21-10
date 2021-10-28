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
        else if (!variable->getType()->isAggregateType()) {
            auto value = generateExpression(ast.back(), context);
            context.builder.CreateStore(value, variable);
            return value;
        }
        else {
            throw InternalError{
                ast.token,
                "assignment of aggregate types not implemented" +
                    std::string{name(ast.type)}};
        }
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

llvm::AllocaInst *generateStructInitializer(Ast &ast, CodegenContext &context) {
    if (ast.size() != 2 || ast.back().type != Token::InitializerList) {
        throw InternalError{ast.token,
                            "bad format on struct initializer" +
                                std::string{name(ast.type)}};
    }

    auto &typeNameAst = ast.get(Token::TypeName);
    auto type = context.scope.getStruct(typeNameAst.token.content);

    auto &list = ast.get(Token::InitializerList);
    groupStandard(list);

    if (list.empty()) {
        return nullptr; // TODO: Handle null initialization in the future
    }

    auto flat = flattenList(list.front());

    auto values = std::vector<llvm::Value *>{};

    if (flat.front()->type == Token::Assignment) {
        for (auto f : flat) {
            // Todo:  Handle the naming correctly
            values.push_back(generateExpression(f->back(), context));
        }
    }
    else {
        for (auto f : flat) {
            values.push_back(generateExpression(*f, context));
        }
    }

    auto function = context.builder.GetInsertBlock()->getParent();
    auto alloca =
        createEntryBlockAlloca(*function, type->name.content, type->type);

    // https://llvm.org/doxygen/classllvm_1_1IRBuilderBase.html#afef76233e8877797902c325bb078e381
    size_t index = 0;
    for (auto value : values) {
        // auto elementType = type->members.at(index).type;
        auto gep = context.builder.CreateStructGEP(
            type->type, alloca, index, type->members.at(index).name.content);
        context.builder.CreateStore(value, gep);
        ++index;
    }

    // https://stackoverflow.com/questions/26787341/inserting-getelementpointer-instruction-in-llvm-ir

    // How do you do to make it work?
    // auto gep = context.builder.CreateGEP(alloca, ids, "tmp");

    // auto gep = context.builder.CreateStructGEP(type->type, alloca, 1,
    // "member");
    // (void)gep;

    return alloca;
}

} // namespace

llvm::Value *generateExpression(Ast &ast, CodegenContext &context) {
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
    case Token::StructInitializer:
        return generateStructInitializer(ast, context);
    default:
        throw InternalError{ast.token,
                            "Could not create expression of type " +
                                std::string{name(ast.type)}};
    }
}
