#include "writeoutputfile.h"
#include "context.h"
#include "log.h"
#include <fstream>

void cpp::writeOutputFile(Context &context, std::filesystem::path path) {
    auto tmpPath = path;
    tmpPath.replace_extension(".tmp");
    {
        auto file = std::ofstream{tmpPath};
        context.dumpCpp(file);
    }

    log("Generating file");
    {
        auto command =
            "g++ -o " + path.string() + " -c -x c++ " + tmpPath.string();
        std::cout << command << std::endl;
        std::system(command.c_str());
    }

    {
        std::cout << "linking..." << std::endl;
        auto nativeLib = " scripts/lib.cpp ";
        auto command = "g++ -o " + (path.parent_path() / path.stem()).string() +
                       " " + nativeLib + path.string();

        std::cout << command << std::endl;
        system(command.c_str());
    }
}
