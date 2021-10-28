
#include <iostream>

struct str {
    char *data = nullptr;
    size_t size = 0;
};

extern "C" void log_print(str *s) {
    std::cout.write(s->data, s->size);
}

extern "C" void log_putc(int c) {
    std::cout.put(c);
}