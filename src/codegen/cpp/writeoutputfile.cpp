#include "writeoutputfile.h"
#include "context.h"
#include "log.h"
#include <fstream>

using std::literals::operator""sv;

constexpr auto builtinTypes = R"_(

#include <cstddef>

struct str {
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

    dlog("Generating file");
    {
        auto command =
            "g++ -o " + path.string() + " -c -x c++ " + tmpPath.string();
        dlog(command);
        std::system(command.c_str());
    }

    {
        dlog("linking...");
        auto nativeLib = " scripts/lib.cpp ";
        auto command = "g++ -o " + (path.parent_path() / path.stem()).string() +
                       " " + nativeLib + path.string();

        dlog(command);
        system(command.c_str());
    }
}
