#include "code/parser.h"
#include "mls-unit-test/unittest.h"

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

TEST_CASE("Nested parentheses") {
    {
        auto ast = parse("({hello})");

        std::cout << ast;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Parentheses);
        EXPECT_EQ(ast.front().size(), 1);
        EXPECT_EQ(ast.front().front().type, Token::Braces);
        EXPECT_EQ(ast.front().front().size(), 1);
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

TEST_CASE("Import") {
    {
        auto ast = parse("import x");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::ImportStatement);
    }

    {
        auto ast = parse("import x\n"
                         "import y");

        EXPECT_EQ(ast.size(), 2);
        EXPECT_EQ(ast.front().type, Token::ImportStatement);
        EXPECT_EQ(ast.back().type, Token::ImportStatement);
    }
}

TEST_CASE("Export") {
    {
        auto ast = parse("export func hello() {}");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::ExportStatement);
    }
}

TEST_CASE("Member accessor") {
    {
        auto ast = parse("apa->x");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::PointerMemberAccessor);
    }
    {
        auto ast = parse("apa.x");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::ValueMemberAccessor);
    }
}

TEST_CASE("module statement") {
    {
        auto ast = parse("module main");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::ModuleStatement);
    }
}

TEST_CASE("binary operators") {
    {
        auto ast = parse("x + y");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::BinaryOperation);
    }

    {
        auto ast = parse("x * y");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::BinaryOperation);
    }
}

TEST_CASE("nested binary operators") {
    {
        auto ast = parse("x + y + z");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::BinaryOperation);
    }
}

TEST_CASE("list") {
    {
        auto ast = parse("x, 2, 3");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::List);
    }
    {
        auto ast = parse("x + y, 2, 3");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::List);
    }
}

TEST_CASE("typed variable") {
    {
        auto ast = parse("x int");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::TypedVariable);
    }
}

TEST_CASE("variable declaration") {
    {
        auto ast = parse("let x int");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::VariableDeclaration);
    }
}

TEST_CASE("assignment") {
    {
        auto ast = parse("x = y");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Assignment);
    }

    {
        auto ast = parse("let x = y");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::Assignment);
    }
}

TEST_CASE("structure") {
    {
        auto ast = parse("struct Apa { x int \n y float }");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::StructDeclaration);
    }
}

TEST_CASE("struct initializer") {
    {
        auto ast = parse("Apa {x = 10, y = 20}");
        groupStandard(ast, true);
        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::StructInitializer);

        std::cout << ast;

        auto list = ast.findRecursive(Token::InitializerList);

        EXPECT_TRUE(list);

        std::cout << *list << std::endl;
        EXPECT_EQ(list->size(), 1);
        EXPECT_EQ(list->front().type, Token::List);

        EXPECT_TRUE(list->findRecursive(Token::Assignment));
    }
}

TEST_CASE("extern statement") {
    {
        auto ast = parse("extern func hello(int, int)");
        std::cout << ast << std::endl;
        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::ExternStatement);
    }

    {
        auto ast = parse("extern \"C\" func hello(int, int)");
        std::cout << ast << std::endl;
        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::ExternStatement);
    }
}

TEST_CASE("function prototype") {
    {
        auto ast = parse("func apa()");
        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::FunctionPrototype);
    }
    {
        auto ast = parse("func apa() {}");
        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::FunctionDeclaration);
    }
}

TEST_CASE("pointer type") {
    {
        auto ast = parse("x int*");

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::PointerTypedVariable);
    }
}

TEST_CASE("pointer types 2") {
    {
        auto ast = parse("struct Apa {x int *\n"
                         "y int\n"
                         "}");
        groupStandard(ast, true);
        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::StructDeclaration);

        //        groupStandard(ast.getRecursive(Token::StructBody));

        std::cout << ast << std::endl;

        // We do not want everything to be a multiplication
        EXPECT_FALSE(ast.findRecursive(Token::BinaryOperation));
    }
}

TEST_CASE("function return type") {
    {
        auto ast = parse("func apa() -> int ");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::TypedFunctionPrototype);
    }
    {
        auto ast = parse("func apa() -> int {}");

        std::cout << ast << std::endl;

        EXPECT_EQ(ast.size(), 1);
        EXPECT_EQ(ast.front().type, Token::TypedFunctionDeclaration);
    }
}

TEST_SUIT_END
