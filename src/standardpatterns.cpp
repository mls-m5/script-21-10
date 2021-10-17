#include "standardpatterns.h"

const Patterns &getStandardPatterns() {
    static const auto patterns = Patterns{
        {
            EqualPriorityPatterns{{
                {
                    {Token::Any, Token::Period, Token::Word},
                    Token::ValueMemberAccessor,
                    {Token::Keep, Token::Keep, Token::MemberName},
                },
                {
                    {Token::Any, Token::RightArrow, Token::Word},
                    Token::PointerMemberAccessor,
                    {Token::Keep, Token::Keep, Token::MemberName},
                },
            }},
        },
        {
            {Token::FuncKeyword,
             Token::Word,
             Token::Parentheses,
             Token::Braces},
            Token::FunctionDeclaration,
            {Token::FuncKeyword,
             Token::Name,
             Token::Parentheses,
             Token::FunctionBody},
        },
        {
            {Token::Any, Token::Parentheses},
            Token::FunctionCall,
        },
        {
            {Token::ImportKeyword, Token::Any},
            Token::ImportStatement,
        },
        {
            {Token::ExportKeyword, Token::Any},
            Token::ExportStatement,
        },
        {
            {Token::ModuleKeyword, Token::Any},
            Token::ModuleStatement,
        },
    };

    return patterns;
}
