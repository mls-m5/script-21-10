#pragma once

#include "code/token.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>
#include <memory>

struct Struct {
    struct StructMember {
        Token name;
        llvm::Type *type;
    };

    size_t getMemberIndex(std::string_view name) {
        for (size_t i = 0; i < members.size(); ++i) {
            auto &member = members.at(i);
            if (member.name.content == name) {
                return i;
            }
        }
        return std::numeric_limits<size_t>::max();
    }

    llvm::StructType *type;
    std::vector<StructMember> members;
    Token name;
};

struct Variable {
    llvm::AllocaInst *alloca;
    llvm::Type *type = nullptr;
};

struct Scope {
    const Scope *parent = 0;

    std::map<std::string, Variable> values;
    std::map<std::string, llvm::Function *> definedFunctions;
    std::map<std::string, Struct> customTypes;

    void defineFunction(Token name, llvm::Function *f) {
        definedFunctions[name.toString()] = f;
    }

    Variable *getVariable(std::string_view name) {
        if (auto f = values.find(std::string{name}); f != values.end()) {
            return &f->second;
        }
        return nullptr;
    }

    Struct *getStruct(std::string_view name);
    void setStruct(std::string_view name, Struct);

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
