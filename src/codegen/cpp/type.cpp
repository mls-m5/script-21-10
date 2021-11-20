#include "type.h"

std::string cpp::SpecificType::toString() const {
    if (!_type) {
        return "void";
    }

    // Todo: Also handlen pointers
    auto str = _type->name;

    if (isReference()) {
        str += "&";
    }

    for (int i = 0; i < pointerDepth(); ++i) {
        str += "*";
    }

    return str;
}
