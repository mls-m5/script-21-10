#include "function.h"
#include "code/parser.h"
#include "context.h"
#include "expression.h"
#include <sstream>

using namespace cpp;

namespace {

std::string join(const std::vector<FunctionPrototype::Arg> &args,
                 char separator,
                 Context &context) {
    std::ostringstream ss;
    for (auto &arg : args) {
        ss << arg.getType(context).toString() << " " << arg.name << separator;
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
                                     bool shouldDisableMangling,
                                     bool isMethod)
    : shouldDisableMangling(shouldDisableMangling)
    , moduleName(moduleName)
    , shouldExport(shouldExport)
    , isMethod(isMethod) {

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
        return astParentheses.empty() ? std::vector<const Ast *>{}
                                      : flattenList(astParentheses.front());
    }();

    for (auto *astArg : astArgs) {
        auto &nameAst = astArg->getRecursive(Token::Name);
        auto &typeAst = astArg->getRecursive(Token::TypeName);
        int pointer = 0;
        if (astArg->type == Token::PointerTypedVariable) {
            pointer = 1;
        }
        args.push_back(
            {nameAst.token.toString(), typeAst.token.toString(), pointer});
    }
}

std::string FunctionPrototype::signature(Context &context) {
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

    ss << "(" << join(args, ' ', context) << ")";

    return ss.str();
}

std::string FunctionPrototype::methodSignature(Context &context,
                                               std::string_view parentName,
                                               bool functionPointer) {
    auto ss = std::ostringstream{};

    ss << returnTypeName << " ";

    if (functionPointer) {
        ss << "(*" << mangledName(parentName) << ")";
    }
    else {
        ss << mangledName(parentName);
    }

    ss << "("
       << "void*"
       << "self, " << join(args, ' ', context) << ")";

    return ss.str();
}

std::string FunctionPrototype::lambdaSignature(Context &context) {
    auto ss = std::ostringstream{};

    ss << "[]";

    ss << "(" << join(args, ' ', context) << ")";

    ss << "->" << returnTypeName;

    return ss.str();
}

std::string FunctionPrototype::mangledName(std::string_view parentName) {
    auto ss = std::ostringstream{};

    if (!shouldDisableMangling && !moduleName.empty() && name != "main") {
        ss << moduleName << "_";
    }

    if (!parentName.empty()) {
        ss << parentName << "_";
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
                                            bool shouldDisableMangling,
                                            bool isMethod) {
    auto function = FunctionPrototype{
        ast, context.moduleName, shouldExport, shouldDisableMangling, isMethod};

    context.functions.emplace(function.localName(), function);

    return function;
}

void generateFunctionDeclaration(const Ast &ast,
                                 Context &context,
                                 bool shouldExport) {
    auto function = generateFunctionPrototype(ast, context, shouldExport);

    auto ss = std::ostringstream{};

    ss << function.signature(context);

    auto oldInsertPoint =
        context.insertBlock({ss.str(), function.location, ast.token.buffer});

    // Todo: Sometime in the far future, optimize this statement
    auto &body = ast.getRecursive(Token::FunctionBody);

    for (auto &arg : function.args) {
        context.pushVariable(Variable{arg.name, {arg.getType(context)}});
    }

    auto lastResult = Value{};
    auto *lastToken = &ast.token;

    for (auto &child : body) {
        lastResult = generateExpression(child, context);
        lastToken = &child.token;
    }

    if (function.returnTypeName != "void") {
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
    }

    context.setInsertPoint(oldInsertPoint);

    for (auto rit = function.args.rbegin(); rit != function.args.rend();
         ++rit) {
        context.popVariable(rit->name);
    }
}

Value generateFunctionCall(const Ast &ast, Context &context) {
    std::vector<Value> args;

    auto &astArgs = ast.get(Token::FunctionArguments);

    if (!astArgs.empty()) {

        auto list = flattenList(astArgs.front());
        for (auto *arg : list) {
            args.push_back(generateExpression(*arg, context));
        }
    }

    auto &target = ast.front();

    auto call = [&context, &ast, &astArgs, &args](FunctionPrototype &function,
                                                  const Token &loc,
                                                  std::string namePrefix = "",
                                                  std::string selfArgument =
                                                      "") {
        if (function.args.size() != args.size()) {
            throw InternalError{ast.token,
                                "trying to call function " +
                                    std::string{function.name} + " with " +
                                    std::to_string(astArgs.size()) +
                                    " arguments, but it expects " +
                                    std::to_string(function.args.size())};
        }
        auto id = context.generateId("fnc");

        auto ss = std::ostringstream{};

        if (!selfArgument.empty()) {
            ss << selfArgument << ",";
        }

        for (auto &arg : args) {
            ss << arg.name << ",";
        }

        auto argsString = ss.str();

        if (!argsString.empty()) {
            argsString.pop_back();
        }

        if (function.returnTypeName == "void") {
            context.insert(
                {namePrefix + function.mangledName() + "(" + argsString + ");",
                 loc});
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

    if (target.type == Token::ValueMemberAccessor ||
        target.type == Token::PointerMemberAccessor) {

        auto &firstAst = target.front();

        auto first = generateExpression(firstAst, context);
        auto type = first.type;

        auto &nameAst = target.back();

        // Trait function calls
        if (auto trait = type.type->traitPtr) {
            auto ss = std::ostringstream{};

            //            ss << first.name;
            //            ss << target.at(1); // "." or "->"
            //            ss << "vtable->";
            //            ss << nameAst.token.content;
            //            ss << first.name << "->ptr";

            if (auto f = trait->methods.find(nameAst.token.toString());
                f != trait->methods.end()) {

                auto &function = f->second;

                auto id = call(function,
                               target.token,
                               first.name + "->vtable->",
                               first.name + "->ptr");

                return {id, function.returnType(context)};
            }
            else {
                throw InternalError{nameAst.token,
                                    "Could not find function " +
                                        nameAst.token.toString() +
                                        " on trait " + trait->name};
            }
        }
        else {

            // Todo: Actually handle the namespacing/module part of this

            if (auto f = context.functions.find(nameAst.token.toString());
                f != context.functions.end()) {

                auto &function = f->second;

                auto id = call(function, target.token);

                return {id, function.returnType(context)};
            }
        }
    }
    throw InternalError{target.token,
                        "Could not find function " +
                            std::string{target.token.content}};
}

SpecificType FunctionPrototype::Arg::getType(Context &context) {
    if (type.type) {
        return type;
    }

    return type = {context.getType(_typeName), pointer};
}

SpecificType FunctionPrototype::Arg::getType(Context &context) const {

    return {context.getType(_typeName), pointer};
}

} // namespace cpp
