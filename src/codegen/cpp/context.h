#pragma once

#include "code/token.h"
#include "function.h"
#include <filesystem>
#include <iosfwd>
#include <list>
#include <map>
#include <string>

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

struct Value {
    // Reference if you want to use the value
    std::string name;
};

struct Type {
    std::string name;

    bool isStruct = false;
};

struct Variable {
    enum Type {
        Value,
        Function,
        Struct,
    };

    Type type = Value;
};

struct Context {
    // insert iterators does not work here since they cannot return the original
    // iterator
    struct InsertPoint {
        Block *block = {};
        Block::ListT::iterator it = {};
    };

    std::filesystem::path filename;

    Block root;

    Context(std::filesystem::path filename);

    std::string generateName(std::string_view base);

    InsertPoint insert(Block);

    void dumpCpp(std::ostream &stream) const;

    Type *getType(std::string_view name);
    Variable *getVariable(std::string_view name);
    void setVariable(std::string name, Variable);
    std::list<FunctionPrototype>::iterator addFunctionPrototype(
        FunctionPrototype);

    // Set and return the old point
    InsertPoint setInsertPoint(InsertPoint it);

    std::string generateId(std::string base = "");

private:
    InsertPoint _insertPoint = InsertPoint{&root, root.lines.end()};

    std::vector<Type> _builtInTypes;
    std::map<std::string, Variable> _variables;
    std::list<FunctionPrototype> _functions;
    std::list<Type> _types;
    size_t _lastId = 0;
};

} // namespace cpp
