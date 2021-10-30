#include "context.h"

namespace cpp {

void Block::dump(std::ostream &stream, int indentSize) const {
    //    auto ident = std::string(indent, ' ');
    auto indent = [&stream](int indentSize) -> std::ostream & {
        for (int i = 0; i < indentSize; ++i) {
            stream << "  ";
        }
        return stream;
    };

    indent(indentSize) << content << "\n";

    if (!lines.empty()) {
        indent(indentSize) << "{\n";
        for (auto &line : lines) {
            line.dump(stream, indentSize + 1);
        }
        indent(indentSize) << "}\n";
    }
}

Context::Context(std::filesystem::path filename)
    : filename(filename)
    , root{Token{}} {}

Context::InsertPoint Context::insert(Block line) {
    auto it =
        _insertPoint.block->lines.insert(_insertPoint.it, std::move(line));
    _insertPoint.it = it;
    ++_insertPoint.it;
    return {_insertPoint.block, it};
}

void Context::dumpCpp(std::ostream &stream) const {
    root.dump(stream);
}

Type *Context::getType(std::string_view name) {
    for (auto &type : _types) {
        if (type.name == name) {
            return &type;
        }
    }

    return nullptr;
}

Variable *Context::getVariable(std::string_view name) {
    if (auto f = _variables.find(std::string{name}); f != _variables.end()) {
        return &f->second;
    }

    return nullptr;
}

decltype(Context::_functions)::iterator Context::addFunctionPrototype(
    FunctionPrototype function) {
    _functions.push_back(std::move(function));
    return --_functions.end();
}

Context::InsertPoint Context::setInsertPoint(InsertPoint it) {
    return std::exchange(_insertPoint, it);
}

std::string Context::generateId(std::string base) {
    if (base.empty()) {
        return "tmp" + std::to_string(_lastId++);
    }
    return std::string{base} + std::to_string(_lastId++);
}

} // namespace cpp
