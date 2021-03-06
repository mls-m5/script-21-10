#pragma once

#ifdef ENABLE_LLVM

#include "code/ast.h"
#include "context.h"

namespace llvmapi {

void importModule(Ast &ast, CodegenContext &context, bool toGlobal);

} // namespace llvmapi

#endif
