#pragma once

#include <string>

namespace cpp {

struct Type {
    std::string name;

    struct Struct *structPtr = nullptr;
    struct Trait *traitPtr = nullptr;
    struct Function *functionPtr = nullptr;
};

// Same as type but also contains pointer level
//  Copied by value
struct SpecificType {
    SpecificType(Type *type, int pointerDepth = 0, bool isReference = false)
        : _type{type}
        , _pointerDepth{pointerDepth}
        , _isReference{isReference} {}

    SpecificType(const SpecificType &) = default;
    SpecificType() = default;

    std::string toString() const;

    bool operator==(const SpecificType &other) const {
        return _type == other._type && _pointerDepth == other._pointerDepth;
    }

    bool operator!=(const SpecificType &other) const {
        return !(*this == other);
    }

    Type *type() const {
        return _type;
    }

    int pointerDepth() const {
        return _pointerDepth - (_type && _type->traitPtr);
    }

    bool isReference() const {
        return _isReference;
    }

private:
    Type *_type = nullptr;
    int _pointerDepth = 0;
    bool _isReference = false;
};

} // namespace cpp
