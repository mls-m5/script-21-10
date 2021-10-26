#include "context.h"
#include <llvm/IR/Function.h>

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function &function,
                                         llvm::StringRef varName,
                                         llvm::Type *type) {
    llvm::IRBuilder<> tmpBuilder(&function.getEntryBlock(),
                                 function.getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}
