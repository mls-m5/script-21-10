#pragma once

#include <filesystem>
#include <string>

// An immutable, unmovable buffer
struct Buffer {
    Buffer(std::string data, std::filesystem::path path)
        : _data(std::move(data))
        , _path(path) {}

    Buffer(const Buffer &) = delete;
    Buffer(Buffer &&) = delete;
    Buffer &operator=(const Buffer &) = delete;
    Buffer &operator=(Buffer &&) = delete;
    Buffer() = delete;

    std::string_view data() const {
        return _data;
    }

    operator std::string_view() const {
        return _data;
    }

    auto size() {
        return _data.size();
    }

    auto begin() {
        return _data.begin();
    }

    auto end() {
        return _data.end();
    }

    auto &at(size_t i) {
        return _data.at(i);
    }

    std::filesystem::path path() {
        return _path;
    }

    // For debug output, find the full line where this string segment is located
    std::string_view getLineAt(std::string_view word) const;

private:
    std::string _data;
    std::filesystem::path _path;
};

std::shared_ptr<Buffer> loadBufferFromFile(std::filesystem::path path);
