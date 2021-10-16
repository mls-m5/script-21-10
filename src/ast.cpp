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
