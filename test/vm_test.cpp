#include "mls-unit-test/unittest.h"
#include "vm/functioncall.h"

using namespace vm;

TEST_SUIT_BEGIN

TEST_CASE("increment int") {
    auto context = Context{};

    context.stack.emplace_back(Variable::TypeT{1});

    auto instructions = InstructionList{};

    instructions.add(std::make_unique<IncrementInt>(VariableLocation{0}));

    instructions.addExit();

    instructions.execute(context);

    EXPECT_EQ(context.stack.at(0).as<int>(), 2);
}

TEST_SUIT_END
