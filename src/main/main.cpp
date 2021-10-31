#include "code/parser.h"
#include "codegen/cpp/module.h"
#include "codegen/cpp/writeoutputfile.h"
#include "codegen/llvmapi/codegen.h"
#include "codegen/llvmapi/import.h"
#include "codegen/llvmapi/writeobjectfile.h"
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

int handleLlvm(filesystem::path out,
               filesystem::path filename,
               const std::vector<filesystem::path> &files) {
    log("generating code");

    std::cout.flush();

    auto context = llvmapi::CodegenContext{filename};

    auto ast = loadAstFromFile(filename);
    try {
        for (auto file : files) {
            log("importing ", file);
            auto ast = loadAstFromFile(file);
            llvmapi::importModule(ast, context, file.stem() == "builtin");
        }
        llvmapi::generateModuleCode(ast, context);
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

    return 0;
};

int handleCpp(filesystem::path out,
              filesystem::path filename,
              const std::vector<filesystem::path> &files) {
    auto ast = loadAstFromFile(filename);

    groupStandard(ast, true);

    log(ast);

    auto context = cpp::Context{filename};

    try {
        cpp::generateModule(ast, context);
        context.dumpCpp(std::cout);
        cpp::writeOutputFile(context, out);
    }
    catch (InternalError &e) {
        log(ast);
        context.dumpCpp(std::cout);
        std::cout << "error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

auto standardImports() {
    auto files = findModuleFiles("log");
    auto builtInFilename = std::filesystem::path{"scripts/builtin.msk"};
    if (!filesystem::exists(builtInFilename)) {
        // Todo: Obviously create some better handling for this
        builtInFilename = "builtin.msk";
    }
    files.insert(files.begin(), builtInFilename);
    return files;
}

int main(int argc, char **argv) {
    std::ios::sync_with_stdio(false);

    if (argc < 2) {
        fatal("to few arguments, please specify file");
    }

    auto filename = std::filesystem::path{argv[1]};
    auto out = filename;
    out.replace_extension(".o");

    auto files = standardImports();

    if (false) {
        return handleLlvm(out, filename, files);
    }
    else {
        return handleCpp(out, filename, files);
    }
}
