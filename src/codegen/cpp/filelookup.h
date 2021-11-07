#pragma once

#include <filesystem>
#include <map>

class FileLookup {
public:
    std::filesystem::path find(const std::string &name) {
        return moduleFiles.at(name);
    }

    FileLookup();

private:
    std::map<std::string, std::filesystem::path> moduleFiles;
};
