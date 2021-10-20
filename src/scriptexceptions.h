#pragma once

#include "code/token.h"
#include <stdexcept>

struct SyntaxError : public std::runtime_error {
    SyntaxError(Token token, std::string error)
        : std::runtime_error{error}
        , token{token} {}
    Token token;
};
