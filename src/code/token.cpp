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
        CASE(AssignmentOperator)

        CASE(FuncKeyword)
        CASE(ImportKeyword)
        CASE(ExportKeyword)
        CASE(ModuleKeyword)
        CASE(LetKeyword)
        CASE(StructKeyword)
        CASE(ExternKeyword)

        CASE(Parentheses)
        CASE(Brackets)
        CASE(Braces)

        CASE(Name)
        CASE(ValueMemberAccessor)
        CASE(PointerMemberAccessor)
        CASE(MemberName)
        CASE(FunctionDeclaration)
        CASE(FunctionPrototype)
        CASE(FunctionCall)
        CASE(FunctionArguments)
        CASE(FunctionBody)
        CASE(StructDeclaration)
        CASE(StructBody)
        CASE(ImportStatement)
        CASE(ExportStatement)
        CASE(ModuleStatement)
        CASE(BinaryOperation)
        CASE(List)
        CASE(VariableDeclaration)
        CASE(TypedVariable)
        CASE(TypeName)
        CASE(Assignment)
        CASE(StructInitializer)
        CASE(InitializerList)
        CASE(ExternStatement)

        CASE(Module)

        CASE(Any)
        CASE(Keep)
        CASE(TypeCount)
    }

    return "<unknown>";
}

#undef CASE
