#include "struct.h"
#include "code/parser.h"
#include "expression.h"
#include "types.h"

llvm::Type *generateStructDeclaration(Ast &ast, CodegenContext &context) {
    auto s = Struct{};
    auto &structNameAst = ast.get(Token::Name);
    s.name = structNameAst.token;

    auto &body = ast.get(Token::StructBody);

    groupStandard(body);

    auto createMember = [&s, &context](Ast &child, bool isPointer) {
        auto &nameAst = child.get(Token::Name);
        auto &typeAst = child.getRecursive(Token::TypeName);
        auto type = getType(typeAst.token, context);
        auto newMember = Struct::StructMember{nameAst.token, type};
        s.members.push_back(newMember);
    };

    for (auto &child : body) {
        if (child.type == Token::TypedVariable) {
            createMember(child, false);
        }
        else if (child.type == Token::PointerTypedVariable) {
            // Todo: Handle pointer types
            createMember(child.front(), true);
        }
        else {
            throw InternalError{child.token,
                                "unexpected expression in struct body " +
                                    std::string{name(child.token.type)}};
        }
    }

    auto types = std::vector<llvm::Type *>{};
    for (auto &m : s.members) {
        types.push_back(m.type);
    }

    auto type = llvm::StructType::create(context.context);
    type->setName(s.name.content);
    type->setBody(types);

    s.type = type;

    context.scope().setStruct(s.name.content, std::move(s));

    return type;
}

// @returns pointer to memory to be used...
llvm::AllocaInst *generateStructInitializer(Ast &ast, CodegenContext &context) {
    if (ast.size() != 2 || ast.back().type != Token::InitializerList) {
        throw InternalError{ast.token,
                            "bad format on struct initializer" +
                                std::string{name(ast.type)}};
    }

    auto &typeNameAst = ast.get(Token::TypeName);
    auto type = context.scope().getStruct(typeNameAst.token.content);

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

    return alloca;
}

// Note: Returns _pointer_ to member value
llvm::Value *generateMemberAccessor(Ast &ast, CodegenContext &context) {
    if (ast.size() != 3) {
        throw InternalError{
            ast.token, "malformed member accessor " + ast.token.toString()};
    }

    auto &variableAst = ast.front();
    auto &memberAst = ast.get(Token::MemberName);

    auto variable = context.scope().getVariable(variableAst.token.content);
    auto structType = getStructFromType(variable->type, context);

    if (!structType) {
        throw InternalError{variableAst.token,
                            "not a struct type: " +
                                variableAst.token.toString()};
    }

    auto memberIndex = structType->getMemberIndex(memberAst.token.content);

    auto gep = context.builder.CreateStructGEP(
        variable->type, variable->alloca, memberIndex, memberAst.token.content);

    return gep;
}