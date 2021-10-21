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

        // Processed values
        Word,
        BeginGroup,
        EndGroup,
        IntLiteral,
        FloatLiteral,
        String,
        Period,
        RightArrow,
        Operator,

        // Keywords
        FuncKeyword,
        ImportKeyword,
        ExportKeyword,
        ModuleKeyword,

        Parentheses,
        Brackets,
        Braces,

        //
        Name,
        ValueMemberAccessor,
        PointerMemberAccessor,
        MemberName,
        FunctionDeclaration,
        FunctionCall,
        FunctionBody,
        ImportStatement,
        ExportStatement,
        ModuleStatement,

        Module,

        // Matchers
        Any,  // used for matching
        Keep, // do not replace with new type when matched
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