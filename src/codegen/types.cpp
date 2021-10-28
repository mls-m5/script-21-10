#include "types.h"
#include "scriptexceptions.h"

llvm::Type *getType(const Token &typeName, CodegenContext &context) {
    auto name = typeName.content;
    if (name == "int") {
        return llvm::Type::getInt32Ty(context.context);
    }
    if (name == "int32") {
        return llvm::Type::getInt32Ty(context.context);
    }
    if (name == "int64") {
        return llvm::Type::getInt64Ty(context.context);
    }
    if (name == "int8") {
        return llvm::Type::getInt8Ty(context.context);
    }
    else if (name == "float") {
        return llvm::Type::getFloatTy(context.context);
    }
    else if (name == "size_t") {
        return llvm::Type::getInt64Ty(context.context);
    }

    auto type = context.scope().getStruct(typeName.content);

    if (!type) {
        throw InternalError{
            typeName, "not recognized type: " + std::string{typeName.content}};
    }

    return type->type;
}

Struct *getStructFromType(llvm::Type *type, Scope &scope) {
    for (auto &pair : scope.customTypes) {
        if (pair.second.type == type) {
            return &pair.second;
        }
    }

    if (scope.parent) {
        return getStructFromType(type, *scope.parent);
    }

    return nullptr;
}