#include "filelookup.h"
#include <fstream>

namespace {

std::string moduleName(std::filesystem::path path) {
    auto file = std::ifstream{path};

    constexpr auto moduleStr = std::string_view{"module "};

    for (std::string line; getline(file, line);) {
        if (line.rfind(moduleStr, 0) != std::string_view::npos) {
            auto moduleName = line.substr(moduleStr.size());
            return moduleName;
        }
    }

    return {};
}

} // namespace

FileLookup::FileLookup() {
    for (auto &it : std::filesystem::recursive_directory_iterator{"."}) {
        if (it.path().extension() != ".msk") {
            continue;
        }
        moduleFiles[moduleName(it.path())] = it.path();
    }
}
