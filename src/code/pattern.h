#pragma once
#include "ast.h"
#include <functional>
#include <vector>

struct Matcher {
    using FuncT = std::function<bool(const Ast &)>;

    static FuncT createTypeFunction(Token::Type type) {
        if (type == Token::Any) {
            return [](const Ast &ast) { return true; };
        }

        return [type](const Ast &ast) { return ast.type == type; };
    }

    Matcher(Token::Type type)
        : f{createTypeFunction(type)}
        , type(type) {
        if (type == Token::Any) {
        }
    }

    Matcher(FuncT f)
        : f(f) {}

    bool operator()(const Ast &ast) const {
        return f(ast);
    }

    FuncT f;
    Token::Type type = Token::None; // Only for debugging
};

struct Pattern {
    enum MatchDirection {
        LeftToRight,
        RightToLeft,
    };

    std::vector<Matcher> matchers;

    // The type to replace the old types with
    // IF you do not want to replace any types, just use a empty vector.
    // IF you want to ignore a specific item, use Token::Keep
    std::vector<Token::Type> newTypes;

    Token::Type result;
    Matcher context = {Token::Any};
    MatchDirection matchDirection = LeftToRight;

    Pattern(std::vector<Matcher> matchers,
            Token::Type result,
            std::vector<Token::Type> newTypes = {},
            MatchDirection matchDirection = LeftToRight,
            Matcher context = {Token::Any})
        : matchers{std::move(matchers)}
        , result(result)
        , newTypes(std::move(newTypes))
        , matchDirection(matchDirection)
        , context(std::move(context)) {
        if (this->newTypes.size() > this->matchers.size()) {
            throw std::runtime_error{
                "trying to create pattern with more new types than matchers"};
        }
    }
};

// Some patterns should not take priority over another
// For example "x.y" and "x->y" should be matched with the same priority
// All of the contained atterns must be matched in the same direction ie left to
// right or right to left. Only the first pattern's read direction is used and
// the others' are assumed to be the same
struct EqualPriorityPatterns {
    std::vector<Pattern> patterns;

    EqualPriorityPatterns(
        std::vector<Matcher> matchers,
        Token::Type result,
        std::vector<Token::Type> newTypes = {},
        Pattern::MatchDirection matchDirection = Pattern::LeftToRight,
        Matcher context = {Token::Any})
        : patterns{{std::move(matchers),
                    std::move(result),
                    std::move(newTypes),
                    matchDirection,
                    std::move(context)}} {};

    EqualPriorityPatterns(std::vector<Pattern> patterns)
        : patterns{std::move(patterns)} {}
};

using Patterns = std::vector<EqualPriorityPatterns>;

// Apply patterns to ast and group accordincly
[[nodiscard]] Ast group(Ast ast, const Patterns &patterns);
