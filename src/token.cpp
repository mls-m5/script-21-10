#include "token.h"

#define CASE(tokenName)                                                        \
    case Token::tokenName:                                                     \
        return #tokenName;

std::string_view name(Token::Type t) {

    switch (t) {
        CASE(None)
        CASE(Space)
        CASE(Numeric)
        CASE(Alpha)
        CASE(NonSpace)
        CASE(Other)

        CASE(Word)
        CASE(BeginGroup)
        CASE(EndGroup)
        CASE(IntLiteral)
        CASE(FloatLiteral)
        CASE(String)

        CASE(FuncKeyword)

        CASE(Parentheses)
        CASE(Brackets)
        CASE(Braces)

        CASE(MemberAccessor)
        CASE(FunctionDeclaration)
        CASE(FunctionCall)

        CASE(Module)

        CASE(Any)
        CASE(TypeCount)
    }

    return "<unknown>";
}

#undef CASE
