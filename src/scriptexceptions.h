#pragma once

#include "code/token.h"
#include <stdexcept>

struct SyntaxError : public std::runtime_error {
    SyntaxError(Token token, std::string error)
        : std::runtime_error{error}
        , token{token} {}

    Token token;
};

struct InternalError : public std::runtime_error {
    InternalError(Token token, std::string error)
        : std::runtime_error{error}
        , token{token} {}

    Token token;
};

struct NotImplementedError : public std::runtime_error {
    NotImplementedError(Token token, std::string error)
        : std::runtime_error{error}
        , token{token} {}

    Token token;
};
