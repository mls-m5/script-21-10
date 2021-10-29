#pragma once

#include "context.h"
#include <filesystem>

namespace llvmapi {

void writeObjectFile(CodegenContext &context, std::filesystem::path filename);

} // namespace llvmapi