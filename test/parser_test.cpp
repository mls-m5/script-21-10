#include "mls-unit-test/unittest.h"
#include "parser.h"

TEST_SUIT_BEGIN

TEST_CASE("Basic parentheses") {
    {
        auto ast = parse("(hello)");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Parentheses);
        EXPECT_EQ(ast.front().size(), 1);
    }

    {
        auto ast = parse("{hello}");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Braces);
        EXPECT_EQ(ast.front().size(), 1);
    }
}

// This is in practice a function call
// TEST_CASE("Multiple parentheses") {
//     {
//         auto ast = parse("(hello)(there)");

//        EXPECT_EQ(ast.size(), 2);

//        EXPECT_EQ(ast.front().type, Token::Parentheses);
//        EXPECT_EQ(ast.front().size(), 1);

//        EXPECT_EQ(ast.back().type, Token::Parentheses);
//        EXPECT_EQ(ast.back().size(), 1);
//    }
//}

TEST_CASE("Nested parentheses") {
    {
        auto ast = parse("({hello})");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Parentheses);
        EXPECT_EQ(ast.front().size(), 1);
    }
}

TEST_CASE("Empty parentheses") {
    {
        auto ast = parse("()");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Parentheses);
        EXPECT_EQ(ast.front().size(), 0);
    }
}

TEST_CASE("Functions") {
    {
        auto ast = parse("func hello() {\n"
                         "  there\n"
                         "}");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::FunctionDeclaration);
    }

    {
        auto ast = parse("hello()");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::FunctionCall);
    }
}

TEST_SUIT_END
