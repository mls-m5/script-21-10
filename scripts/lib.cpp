
#include <iostream>

struct str {
    char *data = nullptr;
    size_t size = 0;
};

extern "C" void native_log_print(str s) {
    std::cout.write(s.data, s.size);
    std::cout << std::endl;
}

extern "C" void native_log_putc(int c) {
    std::cout.put(c);
}
