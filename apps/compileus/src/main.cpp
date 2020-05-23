#include "talkc/entry.h"
#include "talkc/options.h"

// Main is intentionally as minimal of a shim as possible.
int main(int argc, const char* argv[]) {
  auto options = talkc::parse_options(argc, argv);

  options.log_callback = [](talkc::Log_level l, const std::string& msg){
      (void)l;
      std::cerr << msg << "\n";
  };
  return talkc::run(options, std::cin, std::cout);
}