#include "talkc/entry.h"

namespace talkc {

int run(const Options& opts, std::istream& input, std::ostream& output) {
  if (opts.print_version) {
    std::cout << talkc::get_version_string();
    return 0;
  }

  if (opts.print_help) {
    std::cout << talkc::get_usage_string();
    return 0;
  }

  return compile(opts, input, output);
}

}  // namespace talkc