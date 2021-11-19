#include "trait.h"
#include "context.h"

namespace cpp {

Trait::Trait(const Ast &ast, Context &context, bool shouldExport)
    : shouldExport(shouldExport) {
    auto &nameAst = ast.get(Token::Name);

    name = nameAst.token.toString();

    auto &bodyAst = ast.get(Token::TraitBody);

    for (auto &memberAst : bodyAst) {
        if (memberAst.type != Token::FunctionPrototype &&
            memberAst.type != Token::TypedFunctionPrototype) {
            throw InternalError{memberAst.token,
                                "Expected function declaration"};
        }

        auto &nameAst = memberAst.getRecursive(Token::Name);
        auto pair = decltype(Trait::methods)::value_type{
            nameAst.token.toString(),
            generateFunctionPrototype(ast, context, shouldExport, false, true)};

        methods.emplace(std::move(pair));
    }
}

void generateTraitDeclaration(const Ast &ast,
                              Context &context,
                              bool shouldExport) {

    auto trait = Trait{ast, context, shouldExport};

    auto ss = std::ostringstream{};
    ss << "struct " << trait.name;
    auto oldInsertPoint =
        context.insertBlock({ss.str(), ast.token.loc, ast.token.buffer});

    for (auto &method : trait.methods) {
        context.insert(
            {method.second.methodSignature(context, trait.name, true) +
                 " = nullptr;",
             ast.token});
    }

    context.setInsertPoint(oldInsertPoint);
    context.insert({";", ast.token});

    context.setTrait(trait);
}

void generateImplDeclaration(const Ast &ast,
                             Context &context,
                             bool shouldExport) {
    //    auto &traitNameAst = ast.get(Token::Name);
    auto &structNameAst = ast.get(Token::Name);

    //    auto *traitType = context.getType(traitNameAst.token.content);
    //    if (!traitType) {
    //        throw InternalError{traitNameAst.token,
    //                            "Trait " + traitNameAst.token.toString() +
    //                                " is not defined"};
    //    }

    //    if (!traitType->traitPtr) {
    //        throw InternalError{traitNameAst.token,
    //                            "Type " + traitNameAst.token.toString() +
    //                                " is not a trait"};
    //    }

    auto *structType = context.getType(structNameAst.token.toString());
    if (!structType) {
        throw InternalError{structNameAst.token,
                            "struct " + structNameAst.token.toString() +
                                " is not defined"};
    }

    if (!structType->structPtr) {
        throw InternalError{structNameAst.token,
                            "Type " + structNameAst.token.toString() +
                                " is not a struct"};
    }

    auto methodNames = std::vector<std::string>{};

    auto &traitBody = ast.get(Token::ImplBody);

    context.pushVariable({
        // Todo: Handle with selfpointer instead
        "self",
        {structType} // Put some real type here
    });
    auto oldSelf = context.selfStruct(structType->structPtr);

    for (auto &functionAst : traitBody) {
        // Extend to do other stuff than functions
        auto function = generateFunctionDeclaration(
            functionAst, context, shouldExport, false, true);
        //        methodNames.push_back(function.mangledName(structType->name));
        context.insert(
            {function.methodSignature(context, structType->name), ast.token});
    }

    context.popVariable("self");
    context.selfStruct(oldSelf);

    //    auto ss = std::ostringstream{};
    //    ss << "constexpr auto " << traitType->traitPtr->name << "_for_"
    //       << structType->name << " = " << traitType->traitPtr->name;

    //    auto oldInsertPoint =
    //        context.insertBlock({ss.str(), ast.token.loc, ast.token.buffer});

    //    for (auto &name : methodNames) {
    //        context.insert({name + ",", ast.token});
    //    }

    //    context.setInsertPoint(oldInsertPoint);

    context.insert({";", ast.token});
}

} // namespace cpp
