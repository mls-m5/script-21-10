#include "standardpatterns.h"

namespace {
bool isOp5(const Ast &ast) {
    auto str = ast.token.content;
    return ast.type == Token::Operator && str.size() == 1 &&
           (str == "*" || str == "/" || str == "%");
}

bool isOp6(const Ast &ast) {
    auto str = ast.token.content;
    return ast.type == Token::Operator && str.size() == 1 &&
           (str == "+" || str == "-");
}

bool isComa(const Ast &ast) {
    return ast.type == Token::Operator && ast.token.content == ",";
}

bool isRightPointer(const Ast &ast) {
    return ast.type == Token::Operator && ast.token.content == "->";
}

} // namespace

//! I use the c++ operator precedence
//! https://en.cppreference.com/w/cpp/language/operator_precedence
const Patterns &getStandardPatterns() {
    static const auto patterns = Patterns{
        {
            {Token::FuncKeyword,
             Token::Word,
             Token::Parentheses,
             Token::Braces},
            Token::FunctionDeclaration,
            {Token::FuncKeyword,
             Token::Name,
             Token::FunctionArguments,
             Token::FunctionBody},
        },
        EqualPriorityPatterns{
            {
                {
                    {Token::Any, Token::Period, Token::Word},
                    Token::ValueMemberAccessor,
                    {Token::Keep, Token::Keep, Token::MemberName},
                },
                {
                    {Token::Any, {isRightPointer}, Token::Word},
                    Token::PointerMemberAccessor,
                    {Token::Keep, Token::RightArrow, Token::MemberName},
                },
                {
                    {Token::Any, Token::Parentheses},
                    Token::FunctionCall,
                    {Token::Name, Token::FunctionArguments},
                },
            },
        },
        {
            {Token::Any, {isOp5}, Token::Any},
            Token::BinaryOperation,
        },
        {
            {Token::Any, {isOp6}, Token::Any},
            Token::BinaryOperation,
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
            {Token::Keep, Token::Name},
        },
        {
            {Token::Any, {isComa}, Token::Any},
            Token::List,
        },
    };

    return patterns;
}
