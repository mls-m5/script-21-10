#pragma once

#include "code/ast.h"
#include "context.h"
#include "llvm/IR/Module.h"
#include <memory>

//! @returns the final ast
Ast generateModuleCode(Ast, CodegenContext &context);
