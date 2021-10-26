#pragma once

#include "code/token.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>
#include <memory>

struct Variable {
    llvm::AllocaInst *alloca;
    llvm::Type *type = nullptr;
};

struct Scope {
    const Scope *parent = 0;

    std::map<std::string, Variable> values;
    std::map<std::string, llvm::Function *> definedFunctions;

    void defineFunction(Token name, llvm::Function *f) {
        definedFunctions[std::string{name.content}] = f;
    }

    Variable *getVariable(std::string_view name) {
        if (auto f = values.find(std::string{name}); f != values.end()) {
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

    llvm::Function *currentFunction = nullptr;

    Scope scope;

    CodegenContext(std::string id)
        : module{std::make_unique<llvm::Module>(id, context)} {}

    //    inline std::map<std::string, std::unique_ptr<PrototypeAST>>
    //    FunctionProtos;
};

// Push a value and restore when it goes out of scope
template <typename T>
struct PushValue {
    T &value;
    T oldValue;

    PushValue(T &value, T newValue)
        : value(value)
        , oldValue{std::move(value)} {
        value = std::move(newValue);
    }

    ~PushValue() {
        value = std::move(oldValue);
    }
};

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function &function,
                                         llvm::StringRef varName,
                                         llvm::Type *type);
