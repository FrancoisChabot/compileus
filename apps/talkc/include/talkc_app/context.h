#ifndef COMPILEUS_TALKC_OPTIONS_H
#define COMPILEUS_TALKC_OPTIONS_H

#include "spdlog/spdlog.h"

#include <functional>
#include <string>
#include <string_view>

#include "compileus/app_utils/context.h"

namespace talkc {

struct Context : public compileus::app::Context  {
  Context() : compileus::app::Context("talkc") {}

  bool decompile = false;
  std::string decompile_style;
};

std::string_view get_version_string();
std::string_view get_usage_string();

void parse_options(int argc, const char** argv, Context& ctx);
}  // namespace talkc

#endif