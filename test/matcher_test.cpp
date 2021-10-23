#include "code/pattern.h"
#include "mls-unit-test/unittest.h"

TEST_SUIT_BEGIN

TEST_CASE("function pointer") {
    auto isX = +[](const Ast &) { return true; };

    auto isY = +[](const Ast &) { return false; };

    auto m1 = Matcher{isX};
    auto m2 = Matcher{isY};

    EXPECT_TRUE(m1.f(Ast{}));
    EXPECT_FALSE(m2.f(Ast{}));
}

TEST_SUIT_END
