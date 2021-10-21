#pragma once

#include "context.h"
#include <filesystem>

void writeObjectFile(CodegenContext &context, std::filesystem::path filename);
