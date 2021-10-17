// Implementation to just conceptualize how the language would work if it were
// assembled to machine code

#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace vm {

struct Variable {
    using TypeT = std::variant<int, float, std::string>;
    TypeT value;

    Variable(TypeT value)
        : value(value) {}

    template <typename T>
    auto &as() {
        return std::get<T>(value);
    }

    template <typename T>
    auto &as() const {
        return std::get<T>(value);
    }
};

struct Context {
    // In the future the stack does not have to be continious, but could be
    // allocated in smaller blocks
    std::vector<Variable> stack;

    std::vector<Variable> heap;

    size_t currentInstruction = 0;
    size_t stackPointer = 0;
};

struct Instruction {
    virtual ~Instruction() {}

    using ExecuteT = void (*)();

    virtual void execute(Context &) = 0;
    virtual void advance(Context &context) {
        ++context.currentInstruction;
    }
};

struct VariableLocation {
    int address = 0;
    bool onHeap = false;

    VariableLocation(int address = 0, bool onHeap = false)
        : address(address)
        , onHeap(onHeap) {}

    auto &get(Context &context) {
        if (onHeap) {
            return context.heap.at(address);
        }
        else {
            return context.stack.at(address);
        }
    }
};

struct Heap {
    std::vector<Variable> variables;
};

struct IncrementInt : public Instruction {
    VariableLocation variable;

    IncrementInt(VariableLocation variable)
        : variable(variable) {}

    void execute(Context &context) override {
        ++variable.get(context).as<int>();
    }
};

struct FunctionCall : public Instruction {
    ptrdiff_t location = 0;

    void execute(Context &) override {}
    void advance(Context &) override {}
};

struct Exit : public Instruction {
    void execute(Context &context) override {
        context.currentInstruction = std::numeric_limits<size_t>::max();
    }
    void advance(Context &) override {}
};

struct InstructionList {
    // Todo: In the future, reduce indirections to increase performance
    std::vector<std::unique_ptr<Instruction>> data;

    void add(std::unique_ptr<Instruction> instr) {
        data.push_back(std::move(instr));
    }

    void execute(Context &context) {
        for (context.currentInstruction = 0;
             context.currentInstruction !=
             std::numeric_limits<size_t>::max();) {
            auto &instruction = data.at(context.currentInstruction);

            instruction->execute(context);
            instruction->advance(context);
        }
    }

    void addExit() {
        data.push_back(std::make_unique<Exit>());
    }
};

} // namespace vm
