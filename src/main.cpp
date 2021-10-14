

#include "buffer.h"
#include "lexer.h"
#include "log.h"
#include "token.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal("to few arguments, please specify file");
    }

    auto buffer = loadFile(argv[1]);

    log(buffer->data());

    auto tokens = tokenize(buffer);

    for (auto &token : tokens) {
        log(token.content);
    }
}
