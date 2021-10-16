#include "standardpatterns.h"

const Patterns &getStandardPatterns() {
    static const auto patterns = Patterns{
        //        {
        //            EqualPriorityPatterns{{
        //                {
        //                    {Token::Any, Token::Period, Token::Word},
        //                    Token::ValueMemberAccessor,
        //                    {Token::Keep, Token::Keep, Token::MemberName},
        //                },
        //                {
        //                    {Token::Any, Token::RightArrow, Token::Word},
        //                    Token::PointerMemberAccessor,
        //                    {Token::Keep, Token::Keep, Token::MemberName},
        //                },
        //            }},
        //        },
        {
            {Token::FuncKeyword, Token::Any, Token::Parentheses, Token::Braces},
            Token::FunctionDeclaration,
        },
        {
            {Token::Any, Token::Parentheses},
            Token::FunctionCall,
        },
    };

    return patterns;
}
