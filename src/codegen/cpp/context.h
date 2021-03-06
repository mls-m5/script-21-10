#pragma once

#include "code/token.h"
#include "filelookup.h"
#include "function.h"
#include "struct.h"
#include "trait.h"
#include "type.h"
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
        , loc{token.loc}
        , buffer{token.buffer} {}

    Block(std::string content, const Token &copyLocation)
        : content{std::move(content)}
        , loc{copyLocation.loc}
        , buffer{copyLocation.buffer} {}

    Block(std::string content,
          TokenLocation loc,
          std::shared_ptr<Buffer> buffer)
        : content{std::move(content)}
        , loc{loc}
        , buffer(buffer) {}

    std::string content;
    TokenLocation loc;
    std::shared_ptr<Buffer> buffer;

    // List to avoid elements being allocated and deallocated
    ListT lines;

    void dump(std::ostream &stream,
              int indent = 0,
              bool removeLines = false) const;
};

struct Variable {
    std::string name;
    SpecificType type;
};

struct Namespace {
    std::map<std::string, Namespace> subNamespaces;
    std::map<std::string, FunctionPrototype> prototypes;
};

struct Context {
    // insert iterators does not work here since they cannot return the original
    // iterator
    struct InsertPoint {
        Block *block = {};
        Block::ListT::iterator it = {};
    };

    Context();

    Context(Context &) = delete;
    Context &operator=(Context &) = delete;

    std::string_view moduleName();
    void moduleName(std::string);

    std::string generateName(std::string_view base);

    // Insert stuff at point specified by user, and return the line after
    InsertPoint insert(InsertPoint, Block);

    // Insert att the current insertion point
    InsertPoint insert(Block);

    // insert block, return it, set the insert point to it and the old insert
    // point for resetting later
    InsertPoint insertBlock(Block block);

    void dumpCpp(std::ostream &stream, bool removeLines = false) const;

    Type *getType(std::string_view name);

    // Get the pointer including pointer types
    SpecificType getType(const Ast &ast);
    void setType(Type type);
    void setStruct(Struct s);
    void setTrait(Trait t);
    Variable *getVariable(std::string_view name);
    void setVariable(std::string name, Variable);

    // Set function and return pointer to it if insert was successful
    FunctionPrototype *function(std::string name, FunctionPrototype function);
    FunctionPrototype *function(std::string name);

    // Set selfstruct and return the old value
    Struct *selfStruct(Struct *);
    Struct *selfStruct();

    // Set and return the old point
    InsertPoint setInsertPoint(InsertPoint it);

    std::string generateId(std::string base = "");

    void pushVariable(Variable var);

    // ALways do this in the reverse order
    void popVariable(std::string name);

    FileLookup fileLookup;

private:
    Block _root;
    InsertPoint _insertPoint = InsertPoint{&_root, _root.lines.end()};
    // Used to add prefixes to function and type names
    std::string _moduleName;
    // Todo: make private
    std::map<std::string, FunctionPrototype> _functions;

    std::vector<Type> _builtInTypes;
    std::map<std::string, Variable> _variables;
    std::vector<Variable> _variableStack;
    std::list<Type> _types;
    size_t _lastId = 0;
    std::map<std::string, Struct> _structs;
    std::map<std::string, Trait> _traits;
    Struct *_selfStruct = nullptr;
};

} // namespace cpp
