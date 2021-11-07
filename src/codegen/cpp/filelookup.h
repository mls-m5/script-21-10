#pragma once

#include <filesystem>
#include <map>
#include <vector>

class FileLookup {
public:
    std::filesystem::path find(const std::string &name) {
        return moduleFiles.at(name);
    }

    std::vector<std::filesystem::path> findModuleFiles(std::string name);

    FileLookup();

private:
    std::map<std::string, std::filesystem::path> moduleFiles;
};
