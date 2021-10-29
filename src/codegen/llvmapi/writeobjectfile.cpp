#include "writeobjectfile.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

// For more info, checkout
// https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter8/toy.cpp

namespace llvmapi {

void writeObjectFile(CodegenContext &context, std::filesystem::path path) {
    //    llvm::InitializeModuleAndPassManager(); // Could not find

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto targetTripple = llvm::sys::getDefaultTargetTriple();
    context.module->setTargetTriple(targetTripple);

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTripple, error);

    if (!target) {
        llvm::errs() << error;
        throw std::runtime_error{error};
    }

    auto cpu = "generic";
    auto features = "";

    auto opt = llvm::TargetOptions{};
    auto reallocModel = llvm::Optional<llvm::Reloc::Model>{};
    auto targetmachine = target->createTargetMachine(
        targetTripple, cpu, features, opt, reallocModel);

    context.module->setDataLayout(targetmachine->createDataLayout());
    std::error_code ec;
    auto file = llvm::raw_fd_ostream{path.string(), ec, llvm::sys::fs::OF_None};

    if (ec) {
        llvm::errs() << "could not open file: " << ec.message();
        throw std::runtime_error{"could not open file: " + ec.message()};
    }

    llvm::legacy::PassManager pass;
    auto filetype = llvm::CGFT_ObjectFile;

    if (targetmachine->addPassesToEmitFile(pass, file, nullptr, filetype)) {
        auto msg = "target machin can't emit a file of this type";
        llvm::errs() << msg;
        throw std::runtime_error{msg};
    }

    pass.run(*context.module);
    file.flush();
}

} // namespace llvmapi