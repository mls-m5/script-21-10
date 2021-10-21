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
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder{context};
    std::unique_ptr<llvm::Module> module;

    Scope scope;

    CodegenContext(std::string id)
        : module{std::make_unique<llvm::Module>(id, context)} {}

    //    inline std::map<std::string, std::unique_ptr<PrototypeAST>>
    //    FunctionProtos;
};
