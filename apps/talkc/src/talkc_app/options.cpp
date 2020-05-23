#include "talkc/options.h"

namespace talkc {

using namespace std::literals;

std::string_view get_version_string() {
  return "talkc 0.0.1 (" __DATE__ ")\n";
}

std::string_view get_usage_string() {
  return "Usage: talkc [OPTIONS]\n"
         "Converts talk code to compileus representation."
         "\n"
         "Options:\n"
         "    -h, --help        Display this help.\n"
         "    -V, --version     Display version information.\n"
         "    -v, --verbose     Generate more detailed logs.\n"
         "    -d, --decompile   Converts from compileus to talk.\n"
         "    --style=<string>  When decompiling, apply the style specified in "
         "provided file.\n"
         "\n";
}

void parse_options(int argc, const char** argv, Options& dst)
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == "-h"sv || argv[1] == "--help"sv) {
      dst.print_help = true;
    } else if (argv[i] == "-V"sv || argv[1] == "--version"sv) {
      dst.print_version = true;
    } else if (argv[i] == "-v"sv || argv[1] == "--verbose"sv) {
      dst.verbose = true;
    }
  }
}

}  // namespace talkc
