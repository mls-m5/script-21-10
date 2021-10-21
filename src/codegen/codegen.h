#pragma once

#include "code/ast.h"
#include "llvm/IR/Module.h"
#include <memory>

std::unique_ptr<llvm::Module> generateModuleCode(Ast);
