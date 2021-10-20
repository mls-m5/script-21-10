

#include "code/parser.h"
#include "codegen/codegen.h"
#include "log.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal("to few arguments, please specify file");
    }

    auto buffer = loadFile(argv[1]);

    log(buffer->data());

    auto ast = parse(buffer);
    log(ast);
}
