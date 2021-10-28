#include "code/parser.h"
#include "codegen/codegen.h"
#include "codegen/writeobjectfile.h"
#include "log.h"
#include <filesystem>
#include <iostream>

namespace filesystem = std::filesystem;

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal("to few arguments, please specify file");
    }

    auto filename = std::filesystem::path{argv[1]};
    auto out = filename;
    out.replace_extension(".o");

    auto buffer = loadFile(filename);

    if (!buffer) {
        fatal("could not open file ", filename);
    }

    auto ast = parse(buffer);

    log(ast);

    log("generating code");

    std::cout.flush();

    auto context = CodegenContext{filename};

    try {
        generateModuleCode(ast, context);
    }
    catch (SyntaxError &e) {
        log(ast);
        std::cerr << e.what() << "\n";
        return 1;
    }
    catch (InternalError &e) {
        log(ast);
        std::cerr << e.what() << "\n";
        return 1;
    }

    log("after codegen:");
    log(ast);
    std::cout.flush();

    context.module->print(llvm::outs(), nullptr);

    writeObjectFile(context, out);
}
