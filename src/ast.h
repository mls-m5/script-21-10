#pragma once

#include "token.h"

struct Ast {
    Token token;
    Token end;

    Token::Type type = Token::Type::None;
    std::vector<Ast> children;
};
