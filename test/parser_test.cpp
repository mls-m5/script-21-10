#include "mls-unit-test/unittest.h"
#include "parser.h"

TEST_SUIT_BEGIN

TEST_CASE("Basic parenthesis") {
    {
        auto ast = parse("(hello)");

        EXPECT_EQ(ast.children.size(), 1);
        EXPECT_EQ(ast.children.front().type, Token::Parenthesis);
        EXPECT_EQ(ast.children.front().children.size(), 1);
    }

    {
        auto ast = parse("{hello}");

        EXPECT_EQ(ast.children.size(), 1);
        EXPECT_EQ(ast.children.front().type, Token::Braces);
        EXPECT_EQ(ast.children.front().children.size(), 1);
    }
}

TEST_SUIT_END
