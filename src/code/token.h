#pragma once

#include "buffer.h"
#include <memory>
#include <string_view>
#include <vector>

struct TokenLocation {
    int row = 0;
    int col = 0;
};

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
        AssignmentOperator,

        // Keywords
        FuncKeyword,
        ImportKeyword,
        ExportKeyword,
        ModuleKeyword,
        LetKeyword,
        StructKeyword,
        ExternKeyword,

        Parentheses,
        Brackets,
        Braces,

        //
        Name,
        ValueMemberAccessor,
        PointerMemberAccessor,
        MemberName,
        FunctionDeclaration,
        TypedFunctionDeclaration,
        FunctionPrototype,
        TypedFunctionPrototype,
        FunctionCall,
        FunctionArguments,
        FunctionBody,
        StructDeclaration,
        StructBody,
        ImportStatement,
        ExportStatement,
        ModuleStatement,
        BinaryOperation,
        List,
        VariableDeclaration,
        TypedVariable,
        PointerTypedVariable,
        TypeName,
        Assignment,
        StructInitializer,
        InitializerList,
        ExternStatement,

        Module,

        // Matchers
        Any,  // used for matching
        Keep, // do not replace with new type when matched
        TypeCount,
    };

    std::shared_ptr<Buffer> buffer;
    std::string_view content;
    TokenLocation loc;
    std::string_view before = {};
    std::string_view after = {};

    static constexpr TokenLocation nloc = {-1, -1};

    Type type = None;

    bool isEndl() {
        return after.find('\n') != std::string_view::npos;
    }

    std::string toString() const {
        return std::string{content};
    }
};

using Tokens = std::vector<Token>;

std::string_view name(Token::Type);
