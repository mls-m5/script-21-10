#include "context.h"
#include <algorithm>
#include <llvm/IR/Function.h>

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function &function,
                                         llvm::StringRef varName,
                                         llvm::Type *type) {
    llvm::IRBuilder<> tmpBuilder(&function.getEntryBlock(),
                                 function.getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}

Struct *Scope::getStruct(std::string_view name) {
    if (auto f = customTypes.find(std::string{name}); f != customTypes.end()) {
        return &f->second;
    }

    return nullptr;
}

void Scope::setStruct(std::string_view name, Struct s) {
    customTypes[std::string{name}] = std::move(s);
}