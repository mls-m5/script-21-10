#include "type.h"

std::string cpp::SpecificType::toString() const {
    // Todo: Also handlen pointers
    auto str = type->name;

    for (int i = 0; i < pointerDepth; ++i) {
        str += "*";
    }

    return str;
}
