#include "function.h"
#include "code/parser.h"
#include "context.h"
#include "expression.h"
#include <sstream>

using namespace cpp;

namespace {

std::string joinComa(const std::vector<FunctionPrototype::Arg> &args) {
    std::ostringstream ss;
    for (auto &arg : args) {
        ss << arg.type << " " << arg.name << ",";
    }

    auto s = ss.str();

    if (!s.empty()) {
        s.pop_back();
    }

    return s;
}

} // namespace

namespace cpp {

FunctionPrototype::FunctionPrototype(Ast &ast,
                                     std::string_view moduleName,
                                     bool shouldDisableMangling)
    : shouldDisableMangling(shouldDisableMangling)
    , moduleName(moduleName) {

    if (shouldDisableMangling) {
        this->moduleName = "";
    }

    name = ast.get(Token::Name).token.toString();

    auto astArgs = [&] {
        auto &astParentheses = ast.get(Token::FunctionArguments);
        groupStandard(astParentheses);
        return astParentheses.empty() ? std::vector<Ast *>{}
                                      : flattenList(astParentheses.front());
    }();

    for (auto *astArg : astArgs) {
        auto &nameAst = astArg->get(Token::Name);
        auto &typeAst = astArg->get(Token::TypeName);
        args.push_back({nameAst.token.toString(), typeAst.token.toString()});
    }
}

std::string FunctionPrototype::signature() {
    auto ss = std::ostringstream{};

    auto isMain = name == "main";

    if (!isMain) {
        if (shouldDisableMangling) {
            ss << "extern \"C\" ";
        }
        else {
            ss << "static ";
        }
    }

    // Todo: Implement return type
    ss << "int ";

    ss << mangledName();

    ss << "(" << joinComa(args) << ")";

    return ss.str();
}

std::string FunctionPrototype::mangledName() {
    auto ss = std::ostringstream{};

    if (!shouldDisableMangling && !moduleName.empty() && name != "main") {
        ss << moduleName << "_";
    }

    ss << name;

    return ss.str();
}

std::string FunctionPrototype::localName() {
    return name;
}

FunctionPrototype generateFunctionPrototype(Ast &ast,
                                            Context &context,
                                            bool shouldDisableMangling) {
    auto function =
        FunctionPrototype{ast, context.moduleName, shouldDisableMangling};

    context.functions.emplace(function.localName(), function);

    return function;
}

void generateFunctionDeclaration(Ast &ast, Context &context) {
    auto function = generateFunctionPrototype(ast, context);

    std::ostringstream ss;

    ss << function.signature();

    auto block = Block{ss.str(), function.location};
    auto it = context.insert(std::move(block));

    auto oldInsertPoint =
        context.setInsertPoint({&*it.it, it.it->lines.begin()});

    auto &body = ast.get(Token::FunctionBody);

    for (auto &arg : function.args) {
        context.pushVariable(Variable{arg.name});
    }

    auto lastResult = Value{};

    for (auto &child : body) {
        lastResult = generateExpression(child, context);
    }

    generateReturnExpression(lastResult, context);

    context.setInsertPoint(oldInsertPoint);

    for (auto rit = function.args.rbegin(); rit != function.args.rend();
         ++rit) {
        context.popVariable(rit->name);
    }

    (void)it;
}

Value generateFunctionCall(Ast &ast, Context &context) {
    std::vector<Value> args;

    auto &astArgs = ast.get(Token::FunctionArguments);

    if (!astArgs.empty()) {
        groupStandard(astArgs);

        auto list = flattenList(astArgs.front());
        for (auto *arg : list) {
            args.push_back(generateExpression(*arg, context));
        }
    }

    auto &name = ast.get(Token::Name);
    if (auto f = context.functions.find(name.token.toString());
        f != context.functions.end()) {
        auto &function = f->second;

        if (function.args.size() != args.size()) {
            throw InternalError{ast.token,
                                "trying to call function " +
                                    std::string{function.name} + " with " +
                                    std::to_string(astArgs.size()) +
                                    " arguments, but it only has " +
                                    std::to_string(function.args.size())};
        }

        auto id = context.generateId("fnc");

        auto ss = std::ostringstream{};

        for (auto &arg : args) {
            ss << arg.name << ",";
        }

        auto argsString = ss.str();

        if (!argsString.empty()) {
            argsString.pop_back();
        }

        context.insert({"auto " + id + " = " + f->second.mangledName() + "(" +
                            argsString + ");",
                        name.token.loc});

        return {id};
    }
    else {
        throw InternalError{name.token,
                            "Could not find function " +
                                std::string{name.token.content}};
    }
}

} // namespace cpp
