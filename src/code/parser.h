#pragma once

#include "ast.h"
#include "token.h"

[[nodiscard]] Ast parse(Tokens tokens);
[[nodiscard]] Ast parse(std::string code,
                        std::filesystem::path sourcePath = "commandline");
[[nodiscard]] Ast parse(std::shared_ptr<Buffer> buffer);

void groupStandard(Ast &ast, bool isRecursive = false);
