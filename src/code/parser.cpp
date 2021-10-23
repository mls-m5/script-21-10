#include "parser.h"
#include "lexer.h"
#include "scriptexceptions.h"
#include "standardpatterns.h"
#include <algorithm>

namespace {

const std::string_view beginChars = "([{";
const std::string_view endChars = ")]}";

bool doesMatch(char start, char stop) {
    return beginChars.find(start) == endChars.find(stop);
};

// End pointing to the end token
// Used to extract for example expressions like "(expression)"
size_t extractGroup(Ast &ast, size_t begin, size_t end, Token::Type newType) {
    auto newAst = Ast{};
    newAst.type = newType;
    newAst.token = std::move(ast.at(begin).token);

    // Move the inbetween objects
    newAst.resize(end - begin - 1);
    for (auto j = 0; j < newAst.size(); ++j) {
        newAst.at(j) = std::move(ast.at(j + begin + 1));
    }
    newAst.ending = std::move(ast.at(end).token);

    ast.erase(ast.begin() + begin + 1, ast.begin() + begin + newAst.size() + 2);

    ast.at(begin) = std::move(newAst);
    return begin;
}

Token::Type getParanthesisType(const Token &token) {
    if (auto f = beginChars.find(token.content); f != std::string_view::npos) {
        return static_cast<Token::Type>(Token::Parentheses + f);
    }
    else {
        // This should not be possible
        throw std::runtime_error{"trying group non-group, ie not any of '([{'"};
    }
}

// Notice that this presumes that the vector always shrinks and therefore never
// reallocates or moves the data
// Begin is pointing at the index of the first paranthesis, bracket or brace
size_t groupParenthesis(Ast &ast, size_t begin) {

    for (auto i = begin + 1; i < ast.size(); ++i) {
        auto &subAst = ast.at(i);

        if (subAst.type == Token::BeginGroup) {
            i = groupParenthesis(ast, i);
        }
        else if (subAst.type == Token::EndGroup) {
            if (!doesMatch(subAst.token.content.front(),
                           ast.at(begin).token.content.front())) {
                throw SyntaxError{subAst.token,
                                  "Expected matching " +
                                      std::string{ast.token.content}};
            }

            return extractGroup(
                ast, begin, i, getParanthesisType(ast.at(begin).token));
        }
    }

    throw SyntaxError{ast.back().token, "expected end group token"};
}

Ast groupParenthesis(Ast ast) {
    for (size_t i = 0; i < ast.size(); ++i) {
        auto &token = ast.at(i);
        if (token.type == Token::BeginGroup) {
            i = groupParenthesis(ast, i);
        }
    }
    return ast;
}

} // namespace

Ast parse(Tokens tokens) {
    auto ast = Ast{};
    ast.resize(tokens.size());
    std::transform(tokens.begin(), tokens.end(), ast.begin(), [](Token &token) {
        auto ast = Ast{};
        ast.token = token;
        ast.type = token.type;
        return ast;
    });
    ast.type = Token::Module;
    ast = groupParenthesis(std::move(ast));
    ast = groupStandard(std::move(ast));
    return ast;
}

Ast groupStandard(Ast ast) {
    return group(std::move(ast), getStandardPatterns());
}

Ast parse(std::string source) {
    return parse(tokenize(std::move(source)));
}

Ast parse(std::shared_ptr<Buffer> buffer) {
    return parse(tokenize(std::move(buffer)));
}
