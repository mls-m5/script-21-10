#pragma once

#include <filesystem>
#include <string>

// An immutable, unmovable buffer
struct Buffer : public std::enable_shared_from_this<Buffer> {
    Buffer(std::string data)
        : _data(std::move(data)) {}

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

private:
    std::string _data;
};

std::shared_ptr<Buffer> loadFile(std::filesystem::path path);
