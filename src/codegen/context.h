#pragma once

#include "code/token.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>
#include <memory>

struct Variable {
    llvm::Value *value;
    llvm::Type *type = nullptr;
};

struct Scope {
    const Scope *parent = 0;

    std::map<std::string, Variable> values;
    std::map<std::string, llvm::Function *> definedFunctions;

    void defineFunction(Token name, llvm::Function *f) {
        definedFunctions[std::string{name.content}] = f;
    }

    Variable *getVariable(std::string name) {
        if (auto f = values.find(name); f != values.end()) {
            return &f->second;
        }
        return nullptr;
    }

    void clear() {
        values.clear();
        definedFunctions.clear();
    }
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

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function &function,
                                         llvm::StringRef varName,
                                         llvm::Type *type);
