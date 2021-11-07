#pragma once

#include "scriptexceptions.h"
#include "token.h"
#include <iosfwd>

struct Ast : std::vector<Ast> {
    Token token;
    Token ending;
    bool isGrouped = false;

    Token::Type type = Token::Type::None;

    Ast() = default;
    Ast(const Ast &) = delete;
    Ast &operator=(const Ast &) = delete;
    Ast(Ast &&) = default;
    Ast &operator=(Ast &&) = default;

    void print(std::ostream &, int indentation = 0) const;

    const Ast *find(Token::Type type) const {
        for (auto &child : *this) {
            if (child.type == type) {
                return &child;
            }
        }
        return nullptr;
    }

    // Same as find but expect a result
    const Ast &get(Token::Type type) const {
        auto ast = find(type);

        if (!ast) {
            throw SyntaxError{token,
                              std::string{"Could not find "} +
                                  std::string{name(type)} + " in " +
                                  std::string{name(this->type)}};
        }

        return *ast;
    }

    const Ast *findRecursive(Token::Type type) const {
        if (!isGrouped) {
            return nullptr;
        }
        if (this->type == type) {
            return this;
        }
        for (auto &child : *this) {
            //            if (child.type == type) {
            //                return &child;
            //            }

            if (auto f = child.findRecursive(type)) {
                return f;
            }
        }
        return nullptr;
    }

    const Ast &getRecursive(Token::Type type) const {
        auto ast = findRecursive(type);

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

// Coma separated lists are nested expressions. This gives a flat vector
// pointing to all actual values
std::vector<const Ast *> flattenList(const Ast &root);
