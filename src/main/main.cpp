#include "code/parser.h"
#include "codegen/codegen.h"
#include "codegen/writeobjectfile.h"
#include "log.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal("to few arguments, please specify file");
    }

    auto buffer = loadFile(argv[1]);
    auto ast = parse(buffer);

    log(ast);

    log("generating code");

    std::cout.flush();

    auto context = CodegenContext{"modulename"};
    generateModuleCode(std::move(ast), context);

    context.module->print(llvm::outs(), nullptr);

    writeObjectFile(context, "testoutput.o");
}
