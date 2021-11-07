#include "code/parser.h"
#include "codegen/cpp/import.h"
#include "codegen/cpp/module.h"
#include "codegen/cpp/writeoutputfile.h"
#include "log.h"
#include <filesystem>
#include <iostream>

#ifdef ENABLE_LLVM
#include "codegen/llvmapi/codegen.h"
#include "codegen/llvmapi/import.h"
#include "codegen/llvmapi/writeobjectfile.h"
#endif

namespace filesystem = std::filesystem;

Ast loadAstFromFile(filesystem::path filename) {

    auto buffer = loadBufferFromFile(filename);

    if (!buffer) {
        fatal("could not open file ", filename);
    }

    return parse(buffer);
}

int handleLlvm(filesystem::path out, filesystem::path filename) {
#ifdef ENABLE_LLVM
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
        std::cerr << e.token.locationString() << ": " << e.what() << "\n";
        return 1;
    }
    catch (InternalError &e) {
        log(ast);
        std::cerr << e.token.locationString() << ": " << e.what() << "\n";
        return 1;
    }

    log("after codegen:");
    log(ast);
    std::cout.flush();

    context.module->print(llvm::outs(), nullptr);

    writeObjectFile(context, out);

    return 0;
#else
    (void)out;
    (void)filename;
    fatal("llvm is not enabled");
#endif
};

void printErrorInformation(const Token &token, std::string_view message) {
    std::cerr << token.locationString() << ": " << message << "\n";
    auto errStr = std::string{token.buffer->getLineAt(token.content)};
    for (auto &c : errStr) {
        c = (c == '\t') ? ' ' : c;
    }
    std::cerr << errStr << "\n";
    for (int i = 0; i + 1 < token.loc.col; ++i) {
        std::cerr << " ";
    }
    std::cerr << "^-- here\n";
}

auto standardImports(cpp::Context &context) {
    auto files = context.fileLookup.findModuleFiles("log");
    auto builtInFilename = std::filesystem::path{"scripts/builtin.msk"};
    if (!filesystem::exists(builtInFilename)) {
        // Todo: Obviously create some better handling for this
        builtInFilename = "builtin.msk";
    }
    files.insert(files.begin(), builtInFilename);
    return files;
}

int importCpp(cpp::Context &context,
              const std::vector<filesystem::path> &files) {
    for (auto file : files) {
        auto ast = loadAstFromFile(file);

        // Todo: Only do shallow parsing (no function bodies) for imports
        groupStandard(ast, true);
        try {
            dlog("importing ", file);
            cpp::importModule(ast, context, file.stem() == "builtin");
        }
        catch (SyntaxError &e) {
            err(ast);
            printErrorInformation(e.token, e.what());
            return 1;
        }
        catch (InternalError &e) {
            err(ast);
            if (shouldOutputDebugInfo) {
                context.dumpCpp(std::cerr, true);
            }
            printErrorInformation(e.token, e.what());
            return 1;
        }
    }
    return 0;
}

bool inputModule(filesystem::path path, cpp::Context &context) {
    auto ast = loadAstFromFile(path);

    groupStandard(ast, true);

    dlog(ast);
    try {
        cpp::generateModule(ast, context);
        if (shouldOutputDebugInfo) {
            context.dumpCpp(std::cout, true);
        }
    }
    catch (SyntaxError &e) {
        err(ast);
        printErrorInformation(e.token, e.what());
        return 1;
    }
    catch (InternalError &e) {
        err(ast);
        context.dumpCpp(std::cerr, true);
        printErrorInformation(e.token, e.what());
        return 1;
    }
    return 0;
}

int handleCpp(filesystem::path out, std::string moduleName) {

    auto context = cpp::Context{moduleName};

    if (importCpp(context, standardImports(context))) {
        return 1;
    }

    auto standardFiles = standardImports(context);
    auto files = context.fileLookup.findModuleFiles(moduleName);

    for (auto &file : standardFiles) {
        if (inputModule(file, context)) {
            return 1;
        }
    }

    for (auto &file : files) {
        if (inputModule(file, context)) {
            return 1;
        }
    }

    cpp::writeOutputFile(context, out);

    return 0;
}

int main(int argc, char **argv) {
    std::ios::sync_with_stdio(false);

    auto args = std::vector<std::string_view>{argv + 1, argv + argc};

    if (args.size() < 1) {
        fatal("to few arguments, please specify file");
    }

    auto modulename = std::string{argv[1]};

    for (auto arg : args) {
        if (arg == "--debug") {
            shouldOutputDebugInfo = true;
        }
        else {
            modulename = arg;
        }
    }

    auto out = filesystem::path{modulename + ".o"};

    if (false) {
        return handleLlvm(out, modulename);
    }
    else {
        return handleCpp(out, modulename);
    }
}
