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

// Notice that this presumes that the vector always shrinks and therefore never
// reallocates or moves the data
// Begin is pointing at the index of the first paranthesis, bracket or brace
void groupParenthesis(Ast &ast, size_t begin) {
    auto newAst = Ast{};

    newAst.token = std::move(ast.children.at(begin).token);
    if (auto f = beginChars.find(newAst.token.content);
        f != std::string_view::npos) {
        newAst.type = static_cast<Token::Type>(Token::Parenthesis + f);
    }
    else {
        // This should not be possible
        throw std::runtime_error{"trying group non-group, ie not any of '([{'"};
    }

    for (auto i = begin + 1; i < ast.children.size(); ++i) {
        auto &subAst = ast.children.at(i);

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
            newAst.children.resize(i - begin - 1);
            for (auto j = 0; j < newAst.children.size(); ++j) {
                newAst.children.at(j) =
                    std::move(ast.children.at(j + begin + 1));
            }
            newAst.end = std::move(subAst.token);

            ast.children.erase(ast.children.begin() + begin,
                               ast.children.begin() + newAst.children.size() +
                                   2);

            ast.children.insert(ast.children.begin() + begin,
                                std::move(newAst));
            return;
        }
    }

    // Throw error
}

Ast groupParenthesis(Ast ast) {
    for (size_t i = 0; i < ast.children.size(); ++i) {
        auto &token = ast.children.at(i);
        if (token.type == Token::BeginGroup) {
            groupParenthesis(ast, i);
        }
    }
    return ast;
}

Ast parse(Tokens tokens) {
    auto ast = Ast{};
    ast.children.resize(tokens.size());
    std::transform(
        tokens.begin(), tokens.end(), ast.children.begin(), [](Token &token) {
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
