#include "writeoutputfile.h"
#include "context.h"
#include "log.h"
#include <fstream>

using std::literals::operator""sv;

constexpr auto builtinTypes = R"_(

#include <cstddef>

struct str {
//   template <typename T>
   constexpr str(const char s[]): data(s), len(_len(s)) {}

   const char *data;
   size_t len;

   private:
   constexpr size_t _len(const char *s) {
      size_t len = 0;
      for (; s[len] != '\0'; ++len);
      return len;
   }
};

)_";

void cpp::writeOutputFile(Context &context, std::filesystem::path path) {
    auto tmpPath = path;
    tmpPath.replace_extension(".tmp");
    {
        auto file = std::ofstream{tmpPath};
        file << builtinTypes;
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
