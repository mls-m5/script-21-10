#pragma once

#include "scriptexceptions.h"
#include "token.h"
#include <iosfwd>

struct Ast : std::vector<Ast> {
    Token token;
    Token ending;
    bool isGrouped = false;

    Token::Type type = Token::Type::None;

    void print(std::ostream &, int indentation = 0) const;

    Ast *find(Token::Type type) {
        for (auto &child : *this) {
            if (child.type == type) {
                return &child;
            }
        }
        return nullptr;
    }

    // Same as find but expect a result
    Ast &get(Token::Type type) {
        auto ast = find(type);

        if (!ast) {
            throw SyntaxError{token,
                              std::string{"Could not find "} +
                                  std::string{name(type)} + " in " +
                                  std::string{name(this->type)}};
        }

        return *ast;
    }

    friend std::ostream &operator<<(std::ostream &stream, const Ast &ast) {
        ast.print(stream);
        return stream;
    }
};
