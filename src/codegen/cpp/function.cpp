#include "function.h"
#include "code/parser.h"
#include "context.h"
#include "expression.h"
#include <sstream>

using namespace cpp;

namespace {

std::string join(const std::vector<FunctionPrototype::Arg> &args,
                 char separator) {
    std::ostringstream ss;
    for (auto &arg : args) {
        ss << arg.type << " " << arg.name << separator;
    }

    auto s = ss.str();

    if (!s.empty()) {
        s.pop_back();
    }

    return s;
}

const Ast *getFunctionReturnType(const Ast &ast) {
    auto f = ast.findRecursive(Token::TypedFunctionPrototype);

    return f ? &f->back() : nullptr;
}

} // namespace

namespace cpp {

FunctionPrototype::FunctionPrototype(const Ast &ast,
                                     std::string_view moduleName,
                                     bool shouldExport,
                                     bool shouldDisableMangling)
    : shouldDisableMangling(shouldDisableMangling)
    , moduleName(moduleName)
    , shouldExport(shouldExport) {

    if (shouldDisableMangling) {
        this->moduleName = "";
    }

    auto returnTypeAst = getFunctionReturnType(ast);

    if (returnTypeAst) {
        returnTypeName = returnTypeAst->token.toString();
    }

    auto &prototypeAst = ast.getRecursive(Token::FunctionPrototype);

    name = prototypeAst.get(Token::Name).token.toString();

    auto isMain = name == "main";

    if (isMain) {
        returnTypeName = "int";
    }

    auto astArgs = [&] {
        auto &astParentheses = prototypeAst.get(Token::FunctionArguments);
        //        groupStandard(astParentheses);
        return astParentheses.empty() ? std::vector<const Ast *>{}
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
        else if (!shouldExport) {
            ss << "static ";
        }
    }

    ss << returnTypeName << " ";

    ss << mangledName();

    ss << "(" << join(args, ' ') << ")";

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

SpecificType FunctionPrototype::returnType(Context &context) {
    if (!_returnType.type) {
        _returnType.type = context.getType(returnTypeName);
    }

    return _returnType;
}

FunctionPrototype generateFunctionPrototype(const Ast &ast,
                                            Context &context,
                                            bool shouldExport,
                                            bool shouldDisableMangling) {
    auto function = FunctionPrototype{
        ast, context.moduleName, shouldExport, shouldDisableMangling};

    context.functions.emplace(function.localName(), function);

    return function;
}

void generateFunctionDeclaration(const Ast &ast,
                                 Context &context,
                                 bool shouldExport) {
    auto function = generateFunctionPrototype(ast, context, shouldExport);

    std::ostringstream ss;

    ss << function.signature();

    auto block = Block{ss.str(), function.location, ast.token.buffer};
    auto it = context.insert(std::move(block));

    auto oldInsertPoint =
        context.setInsertPoint({&*it.it, it.it->lines.begin()});

    // Todo: Sometime in the far future, optimize this statement
    auto &body = ast.getRecursive(Token::FunctionBody);

    for (auto &arg : function.args) {
        context.pushVariable(Variable{arg.name, {context.getType(arg.type)}});
    }

    auto lastResult = Value{};
    auto *lastToken = &ast.token;

    for (auto &child : body) {
        lastResult = generateExpression(child, context);
        lastToken = &child.token;
    }

    if (!lastResult.name.empty()) {
        auto returnType = function.returnType(context);
        if (lastResult.type.type != returnType.type) {
            throw InternalError(
                *lastToken,
                "return statement does not match function type: " +
                    lastResult.type.type->name + " is not equal to " +
                    returnType.type->name);
        }
        else {
            generateReturnExpression(lastResult, context, *lastToken);
        }
    }

    context.setInsertPoint(oldInsertPoint);

    for (auto rit = function.args.rbegin(); rit != function.args.rend();
         ++rit) {
        context.popVariable(rit->name);
    }

    (void)it;
}

Value generateFunctionCall(const Ast &ast, Context &context) {
    std::vector<Value> args;

    auto &astArgs = ast.get(Token::FunctionArguments);

    if (!astArgs.empty()) {
        //        groupStandard(astArgs);

        auto list = flattenList(astArgs.front());
        for (auto *arg : list) {
            args.push_back(generateExpression(*arg, context));
        }
    }

    auto &target = ast.front();

    auto call = [&context, &ast, &astArgs, &args](FunctionPrototype &function,
                                                  const Token &loc) {
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

        if (function.returnTypeName == "void") {
            context.insert(
                {function.mangledName() + "(" + argsString + ");", loc});
            return std::string{};
        }

        else {
            context.insert({"auto " + id + " = " + function.mangledName() +
                                "(" + argsString + ");",
                            loc});
            return id;
        }
    };

    if (target.type == Token::Word) {
        auto &name = target;
        if (auto f = context.functions.find(name.token.toString());
            f != context.functions.end()) {
            auto &function = f->second;

            auto id = call(function, target.token);

            return {id, function.returnType(context)};
        }
    }

    if (target.type == Token::ValueMemberAccessor) {
        //        auto value = generateExpression(target, context);

        auto &name = target.back();

        // Todo: Actually handle the namespacing/module part of this

        if (auto f = context.functions.find(name.token.toString());
            f != context.functions.end()) {

            auto &function = f->second;

            auto id = call(function, target.token);

            return {id, function.returnType(context)};
        }
    }
    throw InternalError{target.token,
                        "Could not find function " +
                            std::string{target.token.content}};
}

} // namespace cpp
