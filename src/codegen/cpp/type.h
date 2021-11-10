#pragma once

#include <string>

namespace cpp {

struct Type {
    std::string name;

    struct Struct *structPtr = nullptr;
    struct Trait *traitPtr = nullptr;
};

// Same as type but also contains pointer level
//  Copied by value
struct SpecificType {
    Type *type = nullptr;
    int pointerDepth = 0;

    std::string toString() const;

    bool operator==(const SpecificType &other) const {
        return type == other.type && pointerDepth == other.pointerDepth;
    }

    bool operator!=(const SpecificType &other) const {
        return !(*this == other);
    }
};

} // namespace cpp
