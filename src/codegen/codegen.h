#pragma once

#include "code/ast.h"
#include "context.h"
#include <llvm/IR/Module.h>
#include <memory>

//! @returns the final ast
void generateModuleCode(Ast &, CodegenContext &context);
