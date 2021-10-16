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
        CASE(Period)
        CASE(RightArrow)
        CASE(Operator)

        CASE(FuncKeyword)
        CASE(ImportKeyword)
        CASE(ExportKeyword)

        CASE(Parentheses)
        CASE(Brackets)
        CASE(Braces)

        CASE(ValueMemberAccessor)
        CASE(PointerMemberAccessor)
        CASE(MemberName)
        CASE(FunctionDeclaration)
        CASE(FunctionCall)
        CASE(FunctionBody)
        CASE(ImportStatement)
        CASE(ExportStatement)

        CASE(Module)

        CASE(Any)
        CASE(Keep)
        CASE(TypeCount)
    }

    return "<unknown>";
}

#undef CASE
