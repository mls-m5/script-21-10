#pragma once

#include "code/ast.h"
#include "llvm/IR/Module.h"
#include <memory>

inline std::unique_ptr<llvm::Module> generateModuleCode(Ast);
