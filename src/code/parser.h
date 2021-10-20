#pragma once

#include "ast.h"
#include "token.h"

Ast parse(Tokens tokens);
Ast parse(std::string code);
Ast parse(std::shared_ptr<Buffer> buffer);
