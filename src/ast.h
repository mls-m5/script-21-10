#pragma once

#include "token.h"

struct Ast : std::vector<Ast> {
    Token token;
    Token end;

    Token::Type type = Token::Type::None;
};
