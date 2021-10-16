#include "mls-unit-test/unittest.h"
#include "parser.h"

TEST_SUIT_BEGIN

TEST_CASE("Basic parenthesis") {
    {
        auto ast = parse("(hello)");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Parenthesis);
        EXPECT_EQ(ast.front().size(), 1);
    }

    {
        auto ast = parse("{hello}");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Braces);
        EXPECT_EQ(ast.front().size(), 1);
    }
}

TEST_CASE("Multiple parenthesis") {
    {
        auto ast = parse("(hello)(there)");

        EXPECT_EQ(ast.size(), 2);

        EXPECT_EQ(ast.front().type, Token::Parenthesis);
        EXPECT_EQ(ast.front().size(), 1);

        EXPECT_EQ(ast.back().type, Token::Parenthesis);
        EXPECT_EQ(ast.back().size(), 1);
    }
}

TEST_CASE("Nested paranthesis") {
    {
        auto ast = parse("(hello)");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Parenthesis);
        EXPECT_EQ(ast.front().size(), 1);
    }
}

TEST_SUIT_END
