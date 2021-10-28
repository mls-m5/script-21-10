#include "modules.h"
#include <fstream>
#include <iostream>
#include <string_view>

namespace {

bool isModule(std::filesystem::path path, std::string name) {
    auto file = std::ifstream{path};

    constexpr auto moduleStr = std::string_view{"module "};

    for (std::string line; getline(file, line);) {
        if (line.rfind(moduleStr, 0) != std::string_view::npos) {
            auto moduleName = line.substr(moduleStr.size());
            if (moduleName == name) {
                return true;
            }
            else {
                return false;
            }
        }
    }

    return false;
}

} // namespace

std::vector<std::filesystem::path> findModuleFiles(std::string moduleName) {
    auto ret = std::vector<std::filesystem::path>{};
    for (auto it : std::filesystem::recursive_directory_iterator{"."}) {
        if (it.path().extension() != ".msk") {
            continue;
        }
        if (isModule(it.path(), moduleName)) {
            ret.push_back(it.path());
        }
    }

    return ret;
}