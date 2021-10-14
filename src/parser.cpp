#include "parser.h"
#include <algorithm>

// Notice that this presumes that the vector always shrinks and therefore never
// reallocates or moves the data
void groupParenthesis(AstTree &tokens, size_t begin) {
    for (size_t i = begin; i < tokens.size(); ++i) {
        auto &token = tokens.at(i);

        if (token.type == Token::BeginGroup) {
            groupParenthesis(tokens, i + 1);
        }
        else if (token.type == Token::EndGroup) {
        }
    }
}

AstTree groupParenthesis(AstTree tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        auto &token = tokens.at(i);
        if (token.type == Token::BeginGroup) {
            groupParenthesis(tokens, i + 1);
        }
    }
    return tokens;
}

AstTree parse(Tokens tokens) {
    auto astTree = AstTree{tokens.size()};
    std::transform(tokens.begin(),
                   tokens.end(),
                   astTree.begin(),
                   [](auto &token) { return AstTree{token}; });
    astTree = groupParenthesis(std::move(astTree));
    // .. other stuff...
    return astTree;
}
