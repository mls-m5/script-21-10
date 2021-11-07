#pragma once

#include <string>

namespace cpp {

struct Type {
    std::string name;

    struct Struct *structPtr = nullptr;
};

// Same as type but also contains pointer level
//  Copied by value
struct SpecificType {
    Type *type = nullptr;
    int pointerDepth = 0;
};

} // namespace cpp
