#pragma once

#include "type.h"
#include <string>

namespace cpp {

struct Value {
    // Reference if you want to use the value
    std::string name;

    SpecificType type;
};

} // namespace cpp
