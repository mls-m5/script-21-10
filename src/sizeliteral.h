#pragma once

#include <cstddef>

std::size_t operator"" _uz(unsigned long long int x) {
    return x;
}
