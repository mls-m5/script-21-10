#include "buffer.h"
#include <fstream>
#include <sstream>

std::shared_ptr<Buffer> loadFile(std::filesystem::path path) {
    auto file = std::ifstream{path};
    auto buffer = std::ostringstream{};

    buffer << file.rdbuf();

    return std::make_shared<Buffer>(buffer.str());
}
