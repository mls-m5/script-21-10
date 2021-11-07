#include "ast.h"

namespace {} // namespace

void Ast::print(std::ostream &stream, int indentation) const {
    auto indent = [&stream, indentation ]() -> auto & {
        for (int i = 0; i < indentation; ++i) {
            stream << "  ";
        }

        return stream;
    };

    indent() << "'" << token.content << "'    " << name(type) << "\n";

    for (auto &c : *this) {
        c.print(stream, indentation + 1);
    }

    if (!ending.content.empty()) {
        indent() << "'" << ending.content
                 << "'"
                    "\n";
    }
}

std::vector<const Ast *> flattenList(const Ast &root) {
    if (root.type != Token::List) {
        return {&root};
    }

    scriptExpect(root.size() == 3, root.token, "list ast has wrong size");

    auto list = flattenList(root.front());
    list.push_back(&root.back());
    return list;
}
