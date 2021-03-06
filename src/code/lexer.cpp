#include "lexer.h"
#include <algorithm>
#include <map>

using namespace std::literals;

namespace {

constexpr auto beginChars = "([{"sv;
constexpr auto endChars = ")]}"sv;
constexpr auto singleCharOperators = ",.+-/*<>!=&"sv;
constexpr auto multiCharOperators = std::array{
    "->"sv,
    "=="sv,
    "!="sv,
    "<="sv,
    ">="sv,
};

constexpr bool isMultiCharOperator(std::string_view str) {
    for (auto op : multiCharOperators) {
        if (str == op) {
            return true;
        }
    }
    return false;
}

void nextLocation(TokenLocation &loc, char c) {
    if (c == '\n') {
        ++loc.row;
        loc.col = 0;
    }
    else {
        ++loc.col;
    }
}

} // namespace

Token::Type getKeyword(std::string_view str) {
    static const auto keywords = std::map<std::string_view, Token::Type>{
        {"func", Token::FuncKeyword},
        {"import", Token::ImportKeyword},
        {"export", Token::ExportKeyword},
        {"module", Token::ModuleKeyword},
        {"let", Token::LetKeyword},
        {"struct", Token::StructKeyword},
        {"trait", Token::TraitKeyword},
        {"extern", Token::ExternKeyword},
        {"impl", Token::ImplKeyword},
        {"for", Token::ForKeyword},
        {"as", Token::AsKeyword},
    };

    if (auto f = keywords.find(str); f != keywords.end()) {
        return f->second;
    }

    return Token::None;
}

Token::Type getOperator(std::string_view str) {
    static const auto operators = std::map<std::string_view, Token::Type>{
        {".", Token::Period},
    };

    if (auto f = operators.find(str); f != operators.end()) {
        return f->second;
    }

    return Token::None;
}

Tokens moveSpaces(Tokens tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
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
    return tokens;
};

Token readString(std::shared_ptr<Buffer> buffer,
                 size_t &beginning,
                 TokenLocation &loc) {
    Token token;

    for (size_t i = beginning + 1; i < buffer->size(); ++i) {
        auto c = buffer->at(i);
        nextLocation(loc, c);
        if (c == '"') {
            auto str = std::string_view{buffer->begin().base() + beginning,
                                        i - beginning + 1};
            auto token = Token{std::move(buffer), str, loc};
            token.type = Token::String;
            beginning = i;
            return token;
        }
    }

    throw std::runtime_error{"expected ending '\"'"};
}

// First pass of creating tokens
Tokens splitBufferIntoRawTokens(std::shared_ptr<Buffer> buffer) {
    auto tokens = Tokens{};
    tokens.reserve(buffer->size());

    auto currentType = Token::None;
    TokenLocation loc = {.row = 1, .col = 0};

    for (size_t i = 0; i < buffer->size(); ++i) {
        auto c = buffer->at(i);
        auto type = Token::None;

        nextLocation(loc, c);

        if (isspace(c)) {
            type = Token::Space;
        }
        else if (isalpha(c) || c == '_') {
            type = Token::Alpha;
        }
        else if (isdigit(c)) {
            type = Token::Numeric;
        }
        else if (c == '"') {
            tokens.push_back(readString(buffer, i, loc));
            currentType = tokens.back().type;
            continue;
        }
        else if (c == ';') {
            type = Token::Semicolon;
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
            auto token = Token{buffer, {(buffer->begin() + i).base(), 1}, loc};
            token.type = type;
            tokens.emplace_back(std::move(token));
        }

        currentType = type;
    }

    return tokens;
};

Tokens removeEmptyTokens(Tokens tokens) {
    auto it = std::remove_if(tokens.begin(), tokens.end(), [](auto &token) {
        return token.content.empty();
    });

    tokens.erase(it, tokens.end());
    tokens.shrink_to_fit();

    return tokens;
};

Tokens tokenize(std::shared_ptr<Buffer> buffer) {
    auto tokens = splitBufferIntoRawTokens(buffer);
    tokens = moveSpaces(std::move(tokens));
    tokens = removeEmptyTokens(std::move(tokens));

    // Find parenthteses and such
    for (auto &token : tokens) {
        if (beginChars.find(token.content.front()) != std::string_view::npos) {
            token.type = Token::BeginGroup;
        }
        else if (endChars.find(token.content.front()) !=
                 std::string_view::npos) {
            token.type = Token::EndGroup;
        }
    }

    for (auto &token : tokens) {
        if (token.type == Token::Alpha) {
            if (auto keyword = getKeyword(token.content);
                keyword != Token::None) {
                token.type = keyword;
            }
            else {
                token.type = Token::Word;
            }
        }
    }

    for (auto &token : tokens) {
        if (token.type == Token::Other && token.content.size() == 1) {
            if (auto f = singleCharOperators.find(token.content);
                f != std::string_view::npos) {
                token.type = Token::Operator;
            }
        }
    }

    // Todo: Group and handle multi char tokens...

    for (auto &token : tokens) {
        if (token.type == Token::Operator) {
            if (auto op = getOperator(token.content); op != Token::None) {
                token.type = op;
            }
        }
    }

    for (size_t i = 1; i < tokens.size(); ++i) {
        auto &a = tokens.at(i - 1);
        auto &b = tokens.at(i);

        auto merge = [&a, &b] {
            b.content = {a.content.begin(),
                         a.content.size() + b.content.size()};
            b.before = a.before;
            b.loc = a.loc;

            a = {};
        };

        if (a.after.empty() && b.before.empty()) {
            if (a.type == Token::Operator && b.type == Token::Operator) {
                auto str = std::string{a.content};
                str += b.content;

                if (isMultiCharOperator(str)) {
                    merge();
                }
            }

            if (a.type == Token::Word && b.type == Token::Numeric) {
                b.type = a.type;
                merge();
            }

            if (a.type == Token::Word && b.type == Token::Word) {
                merge();
            }
        }
    }

    tokens = removeEmptyTokens(std::move(tokens));

    // TODO: Before this: handle floating point literals
    for (auto &token : tokens) {
        if (token.type == Token::Numeric) {
            token.type = Token::IntLiteral;
        }
    }

    return tokens;
}

Tokens tokenize(std::string source, std::filesystem::path sourcePath) {
    return tokenize(std::make_shared<Buffer>(std::move(source), sourcePath));
}
