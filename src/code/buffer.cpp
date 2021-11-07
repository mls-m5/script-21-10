#include "buffer.h"
#include <fstream>
#include <sstream>

std::shared_ptr<Buffer> loadBufferFromFile(std::filesystem::path path) {
    auto file = std::ifstream{path};
    if (!file.is_open()) {
        return {};
    }
    auto buffer = std::ostringstream{};

    buffer << file.rdbuf();

    return std::make_shared<Buffer>(buffer.str(), path);
}

std::string_view Buffer::getLineAt(std::string_view word) const {
    if (word.data() < _data.data() ||
        word.data() > _data.data() + _data.size()) {
        return "";
    }

    auto begin = word.data();
    auto end = begin;
    for (begin = word.data(); begin >= _data.data(); --begin) {
        if (*begin == '\n') {
            ++begin;
            break;
        }
    }

    for (end = word.data(); end < _data.data() + _data.size(); ++end) {
        if (*end == '\n') {
            break;
        }
    }

    if (end < begin) {
        return "";
    }

    return {begin, static_cast<size_t>(end - begin)};
}
