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

    try {
        ast = generateModuleCode(std::move(ast), context);
    }
    catch (InternalError &e) {
        std::cerr << e.what() << "\n";
        log(ast);
        return 1;
    }

    log("after codegen:");
    log(ast);
    std::cout.flush();

    context.module->print(llvm::outs(), nullptr);

    writeObjectFile(context, "testoutput.o");
}
