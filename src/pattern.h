#pragma once
#include "ast.h"
#include <functional>
#include <vector>

struct Matcher {
    using FuncT = std::function<bool(const Ast &)>;

    static FuncT createTypeFunction(Token::Type type) {
        if (type == Token::Any) {
            return [type](auto &&ast) { return true; };
        }

        return [type](auto &&ast) { return ast.type == type; };
    }

    Matcher(Token::Type type)
        : f{createTypeFunction(type)} {
        if (type == Token::Any) {
        }
    }

    Matcher(FuncT f)
        : f(f) {}

    FuncT f;
};

struct Pattern {
    std::vector<Matcher> matchers;

    Token::Type result;
    Matcher context = {Token::Any};

    Pattern(std::vector<Matcher> matchers,
            Token::Type result,
            Matcher context = {Token::Any})
        : matchers{std::move(matchers)}
        , result(result)
        , context(std::move(context)) {}
};

using Patterns = std::vector<Pattern>;

inline const auto test = Patterns{
    {{Token::Any, Token::Parentheses, Token::Braces},
     Token::FunctionDeclaration},
    {{Token::Any, Token::Parentheses}, Token::FunctionCall},
};
