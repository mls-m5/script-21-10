#include "code/parser.h"
#include "codegen/codegen.h"
#include "codegen/import.h"
#include "codegen/writeobjectfile.h"
#include "log.h"
#include "modules/modules.h"
#include <filesystem>
#include <iostream>

namespace filesystem = std::filesystem;

Ast loadAstFromFile(filesystem::path filename) {

    auto buffer = loadBufferFromFile(filename);

    if (!buffer) {
        fatal("could not open file ", filename);
    }

    return parse(buffer);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal("to few arguments, please specify file");
    }

    auto filename = std::filesystem::path{argv[1]};
    auto out = filename;
    out.replace_extension(".o");

    auto files = findModuleFiles("log");
    auto builtInFilename = std::filesystem::path{"scripts/builtin.msk"};
    files.insert(files.begin(), builtInFilename);

    // log(ast);

    log("generating code");

    std::cout.flush();

    auto context = CodegenContext{filename};

    auto ast = loadAstFromFile(filename);
    try {
        for (auto file : files) {
            log("importing ", file);
            auto ast = loadAstFromFile(file);
            importModule(ast, context, file == builtInFilename);
        }
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
