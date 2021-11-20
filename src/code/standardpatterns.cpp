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

bool isAmperesAnd(const Ast &ast) {
    return ast.type == Token::Operator && ast.token.content == "&";
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

bool isNotAsStatement(const Ast &ast) {
    return ast.type != Token::AsStatement;
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
            // func f(...)
            // If it does not have a body it is only a function prototype
            {Token::FuncKeyword, Token::Word, Token::Parentheses},
            Token::FunctionPrototype,
            {Token::FuncKeyword, Token::Name, Token::FunctionArguments},
        },
        {
            // func f(...) -> int
            {Token::FunctionPrototype, {isRightArrow}, Token::Word},
            Token::TypedFunctionPrototype,
            {Token::Keep, Token::RightArrow, Token::TypeName},
        },
        {
            // func f(...) { ... }
            // Without return type
            {Token::FunctionPrototype, Token::Braces},
            Token::FunctionDeclaration,
            {Token::Keep, Token::FunctionBody},
        },
        {
            // func f(...) -> int { ... }
            {Token::TypedFunctionPrototype, Token::Braces},
            Token::TypedFunctionDeclaration,
            {Token::Keep, Token::FunctionBody},
        },
        {
            // struct Apa { ... }
            {Token::StructKeyword, Token::Word, Token::Braces},
            Token::StructDeclaration,
            {Token::Keep, Token::Name, Token::StructBody},
        },
        {
            // trait Apa { ... }
            {Token::TraitKeyword, Token::Word, Token::Braces},
            Token::TraitDeclaration,
            {Token::Keep, Token::Name, Token::TraitBody},
        },
        {
            // impl Apa { ... }
            {Token::ImplKeyword, Token::Word, Token::Braces},
            Token::ImplDeclaration,
            {Token::Keep, Token::Name, Token::ImplBody},
        },
        {
            // impl Apa -> apa
            {Token::ImplKeyword, Token::Word, {isRightArrow}, Token::Word},
            Token::ImplForward,
            {Token::Keep, Token::Name, Token::RightArrow, Token::Keep},
        },
        {
            // trait Apa
            {Token::TraitKeyword, Token::Word},
            Token::ImplInsideDeclaration,
            {Token::Keep, Token::Name},
        },
        {
            // extern func () {...}
            {Token::ExternKeyword, Token::FunctionPrototype},
            Token::ExternStatement,
        },
        {
            // extern "C" func(...) {...}
            {Token::ExternKeyword, {isCLiteral}, Token::FunctionPrototype},
            Token::ExternStatement,
        },
        EqualPriorityPatterns{{
            {
                // apa.x
                {Token::Any, Token::Period, Token::Word},
                Token::ValueMemberAccessor,
                {Token::Keep, Token::Keep, Token::MemberName},
                Pattern::LeftToRight,
                {isNotInitializerList},
            },
            {
                // apa->x
                {Token::Any, {isRightArrow}, Token::Word},
                Token::PointerMemberAccessor,
                {Token::Keep, Token::RightArrow, Token::MemberName},
            },
            {
                // f(...)
                {Token::Any, Token::Parentheses},
                Token::FunctionCall,
                {Token::Keep, Token::FunctionArguments},
            },
            {
                // Apa {}
                {Token::Word, Token::Braces},
                Token::StructInitializer,
                {Token::TypeName, Token::InitializerList},
            },
        }},
        {
            // x as int
            {{isNotAsStatement}, Token::AsKeyword, Token::Any},
            Token::AsStatement,
            {Token::Keep, Token::Keep, Token::TypeName},
        },
        {
            // x int
            {Token::Word, Token::Word},
            Token::TypedVariable,
            {Token::Name, Token::TypeName},
        },
        {
            // x * y
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
            // import log
            {Token::ImportKeyword, Token::Any},
            Token::ImportStatement,
        },
        {
            // export struct Apa {...}
            {Token::ExportKeyword, Token::Any},
            Token::ExportStatement,
        },
        {
            // module main
            {Token::ModuleKeyword, Token::Any},
            Token::ModuleStatement,
            {Token::Keep, Token::Name},
        },
        {
            // x int*
            {Token::TypedVariable, {isAsterisk}},
            Token::PointerTypedVariable,
        },
        EqualPriorityPatterns{{
            {
                // let x int
                {Token::LetKeyword, Token::TypedVariable},
                Token::VariableDeclaration,
            },
            {
                // let x
                {Token::LetKeyword, Token::Word},
                Token::VariableDeclaration,
                {Token::Keep, Token::Name},
            },
        }},
        {
            // x = y
            {Token::Any, {isAssignmentOperator}, Token::Any},
            Token::Assignment,
            {Token::Keep, Token::Assignment, Token::Keep},
        },
        EqualPriorityPatterns{{
            {
                // x&
                {{isAmperesAnd}, Token::Word},
                Token::ReferencingStatement,
            },
        }},
        {
            // x, y, z
            {Token::Any, {isComa}, Token::Any},
            Token::List,
        },
    };

    return patterns;
}
