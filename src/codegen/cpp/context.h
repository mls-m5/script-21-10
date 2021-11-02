#pragma once

#include "code/token.h"
#include "function.h"
#include "struct.h"
#include "value.h"
#include <filesystem>
#include <iosfwd>
#include <list>
#include <map>
#include <string>
#include <variant>

namespace cpp {

// A collection of lines surronuded by braces
struct Block {
    using ListT = std::list<Block>;

    Block(const Token &token)
        : content{token.toString()}
        , loc{token.loc} {}

    Block(std::string content, TokenLocation loc)
        : content{content}
        , loc{loc} {}

    std::string content;
    TokenLocation loc;

    // List to avoid elements being allocated and deallocated
    ListT lines;

    void dump(std::ostream &stream, int indent = 0) const;
};

struct Type {
    std::string name;

    struct Struct *structPtr = nullptr;
};

struct Variable {
    enum Type {
        Value,
        Function,
        Struct,
    };

    std::string name;
    Type type = Value;
    bool isPointer = false;

    bool isStruct() {
        return ptr.index() == 1;
    }

    bool isValue() {
        return ptr.index() == 2;
    }

    // Void is just to have some unused type
    std::variant<void *, struct Struct *, Type *> ptr = {};
};

struct Context {
    // insert iterators does not work here since they cannot return the original
    // iterator
    struct InsertPoint {
        Block *block = {};
        Block::ListT::iterator it = {};
    };

    std::filesystem::path filename;

    // Used to add prefixes to function and type names
    std::string moduleName;

    Block root;

    Context(std::filesystem::path filename);

    Context(Context &) = delete;
    Context &operator=(Context &) = delete;

    std::string generateName(std::string_view base);

    InsertPoint insert(Block);

    void dumpCpp(std::ostream &stream) const;

    Type *getType(std::string_view name);
    void setType(Type type);
    void setStruct(Struct s);
    Variable *getVariable(std::string_view name);
    void setVariable(std::string name, Variable);

    // Set and return the old point
    InsertPoint setInsertPoint(InsertPoint it);

    std::string generateId(std::string base = "");

    void pushVariable(Variable var);

    // ALways do this in the reverse order
    void popVariable(std::string name);

    std::map<std::string, FunctionPrototype> functions;

private:
    InsertPoint _insertPoint = InsertPoint{&root, root.lines.end()};

    std::vector<Type> _builtInTypes;
    std::map<std::string, Variable> _variables;
    std::vector<Variable> _variableStack;
    std::list<Type> _types;
    size_t _lastId = 0;
    std::map<std::string, Struct> _structs;
};

} // namespace cpp
