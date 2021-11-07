#pragma once

#include <iostream>

template <typename... Args>
void log(Args &&...args) {
    ((std::cout << std::forward<Args>(args)), ...) << "\n";
}

template <typename... Args>
void err(Args &&...args) {
    ((std::cerr << std::forward<Args>(args)), ...) << "\n";
}

template <typename... Args>
[[noreturn]] void fatal(Args &&...args) {
    ((std::cerr << std::forward<Args>(args)), ...) << "\n";

    exit(1);
}
