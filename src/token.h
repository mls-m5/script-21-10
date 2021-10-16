#pragma once

#include "buffer.h"
#include <memory>
#include <string_view>
#include <vector>

struct Token {
    enum Type {
        // Early constants used by lexer
        None,
        Alpha,
        Space,
        Numeric,
        NonSpace,
        Other,

        // More moture values
        Word,
        BeginGroup,
        EndGroup,
        IntLiteral,
        FloatLiteral,
        String,

        Parentheses,
        Brackets,
        Braces,

        Module,
        TypeCount,
    };

    std::shared_ptr<Buffer> buffer;
    std::string_view content;
    std::string_view before;
    std::string_view after;

    Type type = None;

    bool isEndl() {
        return after.find('\n') != std::string_view::npos;
    }
};

using Tokens = std::vector<Token>;

std::string_view name(Token::Type);
