#pragma once

#include "token.h"
#include <iosfwd>

struct Ast : std::vector<Ast> {
    Token token;
    Token ending;

    Token::Type type = Token::Type::None;

    void print(std::ostream &, int indentation = 0) const;

    friend std::ostream &operator<<(std::ostream &stream, const Ast &ast) {
        ast.print(stream);
        return stream;
    }
};
