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

bool isRightArrow(const Ast &ast) {
    return ast.type == Token::Operator && ast.token.content == "->";
}

bool isAssignmentOperator(const Ast &ast) {
    return ast.type == Token::Operator && ast.token.content == "=";
}

bool isNotInitializerList(const Ast &ast) {
    return ast.type != Token::InitializerList;
}

bool isCLiteral(const Ast &ast) {
    return ast.token.type == Token::String && ast.token.content == "\"C\"";
}

bool isAsterisk(const Ast &ast) {
    return ast.token.content == "*";
}

std::function<bool(const Ast &ast)> isNot(Token::Type type) {
    return [type](const Ast &ast) {
        return ast.type != type; //
    };
}

} // namespace

//! I use the c++ operator precedence
//! https://en.cppreference.com/w/cpp/language/operator_precedence
const Patterns &getStandardPatterns() {
    static const auto patterns = Patterns{
        {
            // If it does not have a body it is only a function prototype
            {Token::FuncKeyword, Token::Word, Token::Parentheses},
            Token::FunctionPrototype,
            {Token::FuncKeyword, Token::Name, Token::FunctionArguments},
        },
        {
            {Token::FunctionPrototype, {isRightArrow}, Token::Word},
            Token::TypedFunctionPrototype,
            {Token::Keep, Token::RightArrow, Token::TypeName},
        },
        {
            // Without return type
            {Token::FunctionPrototype, Token::Braces},
            Token::FunctionDeclaration,
            {Token::Keep, Token::FunctionBody},
        },
        {
            {Token::TypedFunctionPrototype, Token::Braces},
            Token::TypedFunctionDeclaration,
            {Token::Keep, Token::FunctionBody},
        },
        {
            {Token::StructKeyword, Token::Word, Token::Braces},
            Token::StructDeclaration,
            {Token::Keep, Token::Name, Token::StructBody},
        },
        {
            {Token::ExternKeyword, Token::FunctionPrototype},
            Token::ExternStatement,
        },
        {
            {Token::ExternKeyword, {isCLiteral}, Token::FunctionPrototype},
            Token::ExternStatement,
        },
        EqualPriorityPatterns{{
            {
                {Token::Any, Token::Period, Token::Word},
                Token::ValueMemberAccessor,
                {Token::Keep, Token::Keep, Token::MemberName},
                Pattern::LeftToRight,
                {isNotInitializerList},
            },
            {
                {Token::Any, {isRightArrow}, Token::Word},
                Token::PointerMemberAccessor,
                {Token::Keep, Token::RightArrow, Token::MemberName},
            },
            {
                {Token::Any, Token::Parentheses},
                Token::FunctionCall,
                {Token::Keep, Token::FunctionArguments},
            },
            {
                {Token::Word, Token::Braces},
                Token::StructInitializer,
                {Token::TypeName, Token::InitializerList},
            },
        }},
        {
            {Token::Word, Token::Word},
            Token::TypedVariable,
            {Token::Name, Token::TypeName},
        },
        {
            {isNot(Token::TypedVariable), {isOp5}, Token::Any},
            Token::BinaryOperation,
            {},
            Pattern::LeftToRight,
            Matcher{isNot(Token::StructBody)}, // Prevent grouping of
                                               // variable declarationsjj
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
            {Token::TypedVariable, {isAsterisk}},
            Token::PointerTypedVariable,
        },
        EqualPriorityPatterns{{
            {
                {Token::LetKeyword, Token::TypedVariable},
                Token::VariableDeclaration,
            },
            {
                {Token::LetKeyword, Token::Word},
                Token::VariableDeclaration,
                {Token::Keep, Token::Name},
            },
        }},
        {
            {Token::Any, {isAssignmentOperator}, Token::Any},
            Token::Assignment,
            {Token::Keep, Token::Assignment, Token::Keep},
        },
        {
            {Token::Any, {isComa}, Token::Any},
            Token::List,
        },
    };

    return patterns;
}
