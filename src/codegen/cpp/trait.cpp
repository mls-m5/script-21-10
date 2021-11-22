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
            generateFunctionPrototype(ast, context, shouldExport, false)};

        methods.emplace(std::move(pair));
    }
}

std::string Trait::mangledName() {
    return name;
}

std::string Trait::vtableName() {
    return name + "_vtable";
}

std::string Trait::vtableNameForStruct(Struct &s) {
    return name + "_for_" + s.name + "_vtable";
}

void generateTraitDeclaration(const Ast &ast,
                              Context &context,
                              bool shouldExport) {

    auto trait = Trait{ast, context, shouldExport};

    {
        auto ss = std::ostringstream{};
        ss << "struct " << trait.vtableName();
        auto oldInsertPoint =
            context.insertBlock({ss.str(), ast.token.loc, ast.token.buffer});

        for (auto &method : trait.methods) {
            context.insert(
                {method.second.methodSignature(context, true) + " = nullptr;",
                 ast.token});
        }

        context.setInsertPoint(oldInsertPoint);
        context.insert({";", ast.token});

        context.setTrait(trait);
    }

    {
        // Define thick pointer
        auto ss = std::ostringstream{};
        ss << "struct " << trait.mangledName();
        auto oldInsertPoint =
            context.insertBlock({ss.str(), ast.token.loc, ast.token.buffer});

        context.insert({"void * ptr;", ast.token});
        context.insert(
            {"const " + trait.vtableName() + " * vtable;", ast.token});

        context.setInsertPoint(oldInsertPoint);
        context.insert({";", ast.token});
    }
}

void generateImplDeclaration(const Ast &ast,
                             Context &context,
                             bool shouldExport) {
    auto &structNameAst = ast.get(Token::Name);

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

    auto oldSelf = context.selfStruct(structType->structPtr);

    for (auto &childAst : traitBody) {
        if (childAst.type == Token::ImplInsideDeclaration) {
            auto &traitNameAst = childAst.get(Token::Name);
            auto type = context.getType(traitNameAst.token.content);
            if (type->traitPtr) {
                structType->structPtr->addTrait(type->traitPtr);
            }
            else {
                throw InternalError{childAst.token,
                                    "Could not find trait " +
                                        traitNameAst.token.toString()};
            }
        }
        else {
            generateFunctionDeclaration(childAst, context, shouldExport, false);
        }
    }

    context.selfStruct(oldSelf);

    for (auto trait : structType->structPtr->traits) {
        auto ss = std::ostringstream{};
        ss << "constexpr auto "
           << trait->vtableNameForStruct(*structType->structPtr) << " = "
           << trait->vtableName();

        auto oldInsertPoint =
            context.insertBlock({ss.str(), ast.token.loc, ast.token.buffer});

        for (auto &it : trait->methods) {
            auto name = it.first;
            auto method = structType->structPtr->getMethod(name);
            if (method) {
                context.insert(
                    {"." + name + " = " + method->mangledName() + ",",
                     ast.token});
            }
            else {
                throw InternalError{
                    ast.token,
                    "Struct " + structType->structPtr->name +
                        " should implement trait " + trait->name +
                        " but does not implement method " + name + "()"};
            }
        }

        context.setInsertPoint(oldInsertPoint);
    }

    context.insert({";", ast.token});
}

} // namespace cpp
