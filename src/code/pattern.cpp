#include "sizeliteral.h"
#include "standardpatterns.h"

namespace {

bool doesMatch(const Ast &ast, const Pattern &pattern, size_t index) {
    for (auto i = 0_uz; i < pattern.matchers.size(); ++i) {
        auto &child = ast.at(i + index);
        auto &matcher = pattern.matchers.at(i);

        if (!matcher(child)) {
            return false;
        }
    }

    return true;
}

// Extract selected children to separate ast node and insert them back into the
// ast
std::pair<Ast, size_t> group(Ast ast, const Pattern &pattern, size_t index) {
    auto newAst = Ast{};
    newAst.type = pattern.result;
    newAst.resize(pattern.matchers.size());
    newAst.isGrouped = true;

    for (auto i = 0_uz; i < pattern.matchers.size(); ++i) {
        newAst.at(i) = std::move(ast.at(i + index));
    }

    for (auto i = 0_uz; i < std::min(pattern.newTypes.size(), newAst.size());
         ++i) {
        auto newType = pattern.newTypes.at(i);
        if (newType == Token::Keep) {
            continue;
        }

        newAst.at(i).type = newType;
    }

    ast.erase(ast.begin() + index + 1,
              ast.begin() + index + pattern.matchers.size());

    ast.at(index) = std::move(newAst);

    return {std::move(ast), index};
}

[[nodiscard]] Ast groupFromLeft(Ast ast, const EqualPriorityPatterns &pattern) {
    size_t minSize = std::numeric_limits<size_t>::max();
    for (auto &p : pattern.patterns) {
        minSize = std::min(minSize, p.matchers.size());
    }

    for (size_t i = 0; i + minSize <= ast.size();) {
        bool foundMatch = false;
        for (auto &p : pattern.patterns) {
            if (!p.context(ast)) {
                continue;
            }
            if (p.result == ast.type) {
                // Do not group the same thing again
                continue;
            }
            if (!(i + p.matchers.size() <= ast.size())) {
                // If patterns are different size we need to check for length
                continue;
            }

            if (doesMatch(ast, p, i)) {
                std::tie(ast, i) = group(std::move(ast), p, i);
                foundMatch = true;
                break; // All groups will be retested in the right order
            }
        }

        // If something is grouped that group need to be tested again, to be
        // able to find nested groups
        if (!foundMatch) {
            ++i;
        }
    }
    return ast;
}

[[maybe_unused]] void verify(const Ast &ast) {
    for (auto &child : ast) {
        if (!child.token.content.empty() && !child.token.buffer) {
            throw std::runtime_error{"Invalid ast when grouping"};
        }

        verify(child);
    }
}

} // namespace

void group(Ast &ast, const Patterns &patterns, bool isRecursive) {
    if (ast.isGrouped && !isRecursive) {
        return;
    }

    if (ast.size() > 1) {

        for (auto &pattern : patterns) {
            if (pattern.patterns.empty()) {
                continue;
            }

            if (pattern.patterns.front().matchDirection ==
                Pattern::LeftToRight) {
                ast = groupFromLeft(std::move(ast), pattern);
            }
            else {
                throw std::runtime_error{
                    "group from right not implemented yet"};
            }
        }
    }

    if (isRecursive) {
        for (auto &child : ast) {
            group(child, patterns, isRecursive);
        }
    }

#ifdef VERIFY_EXTRA

    verify(ast);

#endif
    ast.isGrouped = true;
}
