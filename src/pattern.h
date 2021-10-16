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

// Some patterns should not take priority over another
// For example "x.y" and "x->y" should be matched with the same priority
struct EqualPriorityPatterns {
    std::vector<Pattern> patterns;

    EqualPriorityPatterns(std::vector<Matcher> matchers,
                          Token::Type result,
                          Matcher context = {Token::Any})
        : patterns{
              std::move(matchers), result(result), context(std::move(context))};

    EqualPriorityPatterns(std::vector<Pattern> patterns)
        : patterns{std::move(patterns)} {}
};

using Patterns = std::vector<EqualPriorityPatterns>;

inline const auto test = Patterns{
    {
        EqualPriorityPatterns{{
            {
                {Token::Any, Token::Period, Token::Any},
                Token::ValueMemberAccessor,
            },
            {
                {Token::Any, Token::RightArrow, Token::Any},
                Token::PointerMemberAccessor,
            },
        }},
    },
    {
        {Token::Any, Token::Parentheses, Token::Braces},
        Token::FunctionDeclaration,
    },
    {
        {Token::Any, Token::Parentheses},
        Token::FunctionCall,
    },
};
