#include "function.h"
#include "code/parser.h"
#include "context.h"
#include "expression.h"

namespace cpp {

FunctionPrototype::FunctionPrototype(Ast &ast) {
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

FunctionPrototype &generateFunctionProototype(Ast &ast, Context context) {
    return *context.addFunctionPrototype({ast});
}

void generateFunctionDeclaration(Ast &ast, Context &context) {
    auto &function = generateFunctionProototype(ast, context);
    auto block =
        Block{"void " + std::string{function.name} + "()", function.location};
    auto it = context.insert(std::move(block));

    auto oldInsertPoint =
        context.setInsertPoint({&*it.it, it.it->lines.begin()});

    auto &body = ast.get(Token::FunctionBody);

    groupStandard(body);

    auto lastResult = Value{};

    for (auto &child : body) {
        lastResult = generateExpression(child, context);
    }

    generateReturnExpression(lastResult, context);

    context.setInsertPoint(oldInsertPoint);

    (void)it;
}

} // namespace cpp
