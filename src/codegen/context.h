#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>
#include <memory>

struct Scope {
    const Scope *parent = 0;

    std::map<std::string, llvm::Value *> values;
};

struct CodegenContext {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    Scope scope;

    //    inline std::map<std::string, std::unique_ptr<PrototypeAST>>
    //    FunctionProtos;
};
