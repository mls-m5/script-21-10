#include "context.h"

namespace cpp {

void Block::dump(std::ostream &stream, int indentSize, bool removeLines) const {
    auto indent = [&stream](int indentSize) -> std::ostream & {
        for (int i = 0; i < indentSize; ++i) {
            stream << "  ";
        }
        return stream;
    };

    //    if (buffer && !removeLines) {
    //        indent(indentSize) << "#line " << loc.row << " " << buffer->path()
    //                           << "\n";
    //    }
    indent(indentSize) << content << "\n";

    if (!lines.empty()) {
        if (indentSize > 0) {
            indent(indentSize) << "{\n";
        }
        for (auto &line : lines) {
            line.dump(stream, indentSize + 1, removeLines);
        }
        if (indentSize > 0) {
            indent(indentSize) << "}\n";
        }
    }
}

Context::Context()
    : _root{Token{}} {

    // Built in types
    _types.push_back({"void"});
    _types.push_back({"int"});
    _types.push_back({"float"});
    _types.push_back({"uint8"});
    _types.push_back({"size_t"});
}

std::string_view Context::moduleName() {
    return _moduleName;
}

void Context::moduleName(std::string name) {
    _moduleName = std::move(name);
}

Context::InsertPoint Context::insert(InsertPoint insertPoint, Block block) {
    auto it =
        _insertPoint.block->lines.insert(insertPoint.it, std::move(block));
    insertPoint.it = it;
    ++_insertPoint.it;
    return {insertPoint.block, it};
}

Context::InsertPoint Context::insert(Block line) {
    auto it =
        _insertPoint.block->lines.insert(_insertPoint.it, std::move(line));
    _insertPoint.it = it;
    ++_insertPoint.it;
    return {_insertPoint.block, it};
}

void Context::dumpCpp(std::ostream &stream, bool removeLines) const {
    _root.dump(stream, removeLines);
}

Type *Context::getType(std::string_view name) {
    for (auto &type : _types) {
        if (type.name == name) {
            return &type;
        }
    }

    return nullptr;
}

SpecificType Context::getType(const Ast &ast) {
    auto &typeAst = ast.getRecursive(Token::TypeName);
    auto type = getType(typeAst.token.content);
    int pointer = 0;
    if (ast.type == Token::TypedVariable) {
        pointer = 1;
    }

    return {type, pointer};
}

void Context::setType(Type type) {
    _types.push_back(std::move(type));
}

void Context::setStruct(Struct s) {
    auto name = s.name;
    auto &sref = _structs[name] = std::move(s);
    _types.push_back(Type{
        .name = name,
        .structPtr = &sref,
    });
}

void Context::setTrait(Trait t) {
    auto name = t.name;
    auto pair = _traits.insert({t.name, std::move(t)});
    _types.push_back(Type{
        .name = name,
        .traitPtr = &pair.first->second,
    });
}

Variable *Context::getVariable(std::string_view name) {
    if (auto f = _variables.find(std::string{name}); f != _variables.end()) {
        return &f->second;
    }

    return nullptr;
}

FunctionPrototype *Context::function(std::string name,
                                     FunctionPrototype function) {
    auto it = _functions.insert(std::make_pair(name, std::move(function)));
    if (!it.second) {
        return nullptr;
    }
    return &it.first->second;
}

FunctionPrototype *Context::function(std::string name) {
    if (auto f = _functions.find(name); f != _functions.end()) {
        return &f->second;
    }
    return nullptr;
}

Struct *Context::selfStruct(Struct *self) {
    return std::exchange(_selfStruct, self);
}

Struct *Context::selfStruct() {
    return _selfStruct;
}

Context::InsertPoint Context::setInsertPoint(InsertPoint it) {
    return std::exchange(_insertPoint, it);
}

Context::InsertPoint Context::insertBlock(Block block) {
    auto it = insert(std::move(block));
    auto oldInsertPoint = setInsertPoint({&*it.it, it.it->lines.begin()});
    return oldInsertPoint;
}

std::string Context::generateId(std::string base) {
    if (base.empty()) {
        return "tmp" + std::to_string(_lastId++);
    }
    return std::string{base} + std::to_string(_lastId++);
}

void Context::pushVariable(Variable var) {
    if (auto f = _variables.find(var.name); f != _variables.end()) {
        _variableStack.emplace_back(std::move(f->second));
        f->second = std::move(var);
    }
    else {
        _variables[var.name] = var;
    }
}

void Context::popVariable(std::string name) {
    if (!_variableStack.empty() && _variableStack.back().name == name) {
        _variables[name] = _variableStack.back();
        _variableStack.pop_back();
    }
    else {
        _variables.erase(name);
    }
}

} // namespace cpp
