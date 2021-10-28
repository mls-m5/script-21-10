#pragma once

#include "code/token.h"
#include "context.h"
#include <llvm/IR/Type.h>

llvm::Type *getType(const Token &typeName, CodegenContext &context);

Struct *getStructFromType(llvm::Type *type, Scope &scope);
