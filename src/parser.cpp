#include "parser.h"
#include "lexer.h"
#include "scriptexceptions.h"
#include <algorithm>

namespace {

const std::string_view beginChars = "([{";
const std::string_view endChars = ")]}";

bool doesMatch(char start, char stop) {
    return beginChars.find(start) == endChars.find(stop);
};

} // namespace

// void extractGroup(Ast &ast, size_t begin, size_t end) {

//}

// Notice that this presumes that the vector always shrinks and therefore never
// reallocates or moves the data
// Begin is pointing at the index of the first paranthesis, bracket or brace
void groupParenthesis(Ast &ast, size_t begin) {
    auto newAst = Ast{};

    newAst.token = std::move(ast.at(begin).token);
    if (auto f = beginChars.find(newAst.token.content);
        f != std::string_view::npos) {
        newAst.type = static_cast<Token::Type>(Token::Parenthesis + f);
    }
    else {
        // This should not be possible
        throw std::runtime_error{"trying group non-group, ie not any of '([{'"};
    }

    for (auto i = begin + 1; i < ast.size(); ++i) {
        auto &subAst = ast.at(i);

        if (subAst.type == Token::BeginGroup) {
            groupParenthesis(ast, i + 1);
        }
        else if (subAst.type == Token::EndGroup) {
            if (!doesMatch(subAst.token.content.front(),
                           newAst.token.content.front())) {
                throw SyntaxError{subAst.token,
                                  "Expected matching " +
                                      std::string{ast.token.content}};
            }

            // Move the inbetween objects
            newAst.resize(i - begin - 1);
            for (auto j = 0; j < newAst.size(); ++j) {
                newAst.at(j) = std::move(ast.at(j + begin + 1));
            }
            newAst.end = std::move(subAst.token);

            ast.erase(ast.begin() + begin, ast.begin() + newAst.size() + 2);

            ast.insert(ast.begin() + begin, std::move(newAst));
            return;
        }
    }

    throw SyntaxError{ast.back().token, "expected end group token"};
}

Ast groupParenthesis(Ast ast) {
    for (size_t i = 0; i < ast.size(); ++i) {
        auto &token = ast.at(i);
        if (token.type == Token::BeginGroup) {
            groupParenthesis(ast, i);
        }
    }
    return ast;
}

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
    // .. other stuff...
    return ast;
}

Ast parse(std::string source) {
    return parse(tokenize(std::move(source)));
}
