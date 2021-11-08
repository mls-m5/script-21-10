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
        CASE(Semicolon)

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
        CASE(TraitKeyword)
        CASE(ExternKeyword)

        CASE(Parentheses)
        CASE(Brackets)
        CASE(Braces)

        CASE(Name)
        CASE(ValueMemberAccessor)
        CASE(PointerMemberAccessor)
        CASE(MemberName)
        CASE(FunctionDeclaration)
        CASE(TypedFunctionDeclaration)
        CASE(FunctionPrototype)
        CASE(TypedFunctionPrototype)
        CASE(FunctionCall)
        CASE(FunctionArguments)
        CASE(FunctionBody)
        CASE(StructDeclaration)
        CASE(StructBody)
        CASE(TraitDeclaration)
        CASE(TraitBody)
        CASE(ImportStatement)
        CASE(ExportStatement)
        CASE(ModuleStatement)
        CASE(BinaryOperation)
        CASE(List)
        CASE(VariableDeclaration)
        CASE(TypedVariable)
        CASE(PointerTypedVariable)
        CASE(TypeName)
        CASE(Assignment)
        CASE(StructInitializer)
        CASE(InitializerList)
        CASE(ExternStatement)
        CASE(ReferencingStatement)

        CASE(Module)

        CASE(Any)
        CASE(Keep)
        CASE(TypeCount)
    }

    return "<unknown>";
}

#undef CASE

std::string Token::locationString() const {
    auto ss = std::ostringstream{};
    if (buffer) {
        ss << buffer->path().string();
    }
    else {
        ss << "no filename";
    }
    ss << ":" << loc.row << ":" << loc.col;
    return ss.str();
}
