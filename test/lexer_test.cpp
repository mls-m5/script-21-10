#include "code/lexer.h"
#include "mls-unit-test/unittest.h"

TEST_SUIT_BEGIN

TEST_CASE("parenthesis") {
    auto tokens = tokenize("(hello)");

    EXPECT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].content, "(");
    EXPECT_EQ(tokens[1].content, "hello");
    EXPECT_EQ(tokens[2].content, ")");
}

TEST_CASE("keyword") {
    auto tokens = tokenize("func hello");

    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, Token::FuncKeyword);
    EXPECT_EQ(tokens[1].type, Token::Word);
}

TEST_CASE("operator") {
    auto tokens = tokenize("x + y");

    EXPECT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, Token::Word);
    EXPECT_EQ(tokens[1].type, Token::Operator);
    EXPECT_EQ(tokens[2].type, Token::Word);
}

TEST_SUIT_END
