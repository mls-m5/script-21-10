#include "lexer.h"
#include <algorithm>

namespace {

const std::string_view beginChars = "([{";
const std::string_view endChars = ")]}";

} // namespace

Tokens tokenize(std::shared_ptr<Buffer> buffer) {
    auto tokens = Tokens{};
    tokens.reserve(buffer->size());

    auto currentType = Token::None;

    for (auto i = 0; i < buffer->size(); ++i) {
        auto c = buffer->at(i);
        auto type = Token::None;
        if (isspace(c)) {
            type = Token::Space;
        }
        else if (isalpha(c)) {
            type = Token::Alpha;
        }
        else if (isdigit(c)) {
            type = Token::Numeric;
        }
        else {
            type = Token::Other;
        }

        // Group similar types
        if (type != Token::Other && type == currentType) {
            auto &back = tokens.back().content;

            // Extend the token with one character
            back = std::string_view{back.data(), back.size() + 1};
        }
        else {
            auto token = Token{buffer, {(buffer->begin() + i).base(), 1}};
            token.type = type;
            tokens.emplace_back(std::move(token));
        }

        currentType = type;
    }

    //     Move spaces to the next token
    for (auto i = 0; i < tokens.size(); ++i) {
        auto &token = tokens.at(i);
        if (token.type == Token::Space && i < tokens.size() - 1) {
            tokens.at(i + 1).before = token.content;
            token.content = {};
        }
    }

    if (tokens.back().type == Token::Space) {
        tokens.at(tokens.size() - 2).after = tokens.back().content;
        tokens.pop_back();
    }

    // Remove empty tokens
    auto it = std::remove_if(tokens.begin(), tokens.end(), [](auto &token) {
        return token.content.empty();
    });

    tokens.erase(it, tokens.end());

    // Find parenthtesis and such
    for (auto &token : tokens) {
        if (beginChars.find(token.content.front()) != std::string_view::npos) {
            token.type = Token::BeginGroup;
        }
        else if (endChars.find(token.content.front()) !=
                 std::string_view::npos) {
            token.type = Token::EndGroup;
        }
    }

    tokens.shrink_to_fit();
    return tokens;
}

Tokens tokenize(std::string source) {
    return tokenize(std::make_shared<Buffer>(std::move(source)));
}
