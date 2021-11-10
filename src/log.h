#pragma once

#include <iostream>

inline bool shouldOutputDebugInfo = false;

template <typename... Args>
void log(Args &&...args) {
    ((std::cout << std::forward<Args>(args)), ...) << "\n";
}

template <typename... Args>
void dlog(Args &&...args) {
    if (shouldOutputDebugInfo) {
        ((std::cout << std::forward<Args>(args)), ...) << std::endl;
    }
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
