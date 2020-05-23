#ifndef COMPILEUS_TALKC_ENTRY_H
#define COMPILEUS_TALKC_ENTRY_H

#include <iostream>

#include "talkc/options.h"

namespace talkc {
  int run(const Options& opts, std::istream& input, std::ostream& output);
  int compile(const Options& opts, std::istream& input, std::ostream& output);
}  // namespace talkc

#endif