#include "types.h"
#include "scriptexceptions.h"

llvm::Type *getType(const Token &typeName, CodegenContext &context) {
    if (typeName.content == "int") {
        return llvm::Type::getInt32Ty(context.context);
    }
    if (typeName.content == "int32") {
        return llvm::Type::getInt32Ty(context.context);
    }
    if (typeName.content == "int64") {
        return llvm::Type::getInt64Ty(context.context);
    }
    else if (typeName.content == "float") {
        return llvm::Type::getFloatTy(context.context);
    }

    auto type = context.scope.getStruct(typeName.content);

    if (!type) {
        throw InternalError{
            typeName, "not recognized type: " + std::string{typeName.content}};
    }

    return type->type;
}
