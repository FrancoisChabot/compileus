#include "compileus/libtalk/utils.h"

#ifndef WIN32
#include <cxxabi.h>
#endif

#include <cassert>
#include <limits>

namespace compileus {
auto demangle(const std::string& mangled) -> std::string {
  int status = 0;
#ifndef WIN32
  char* demangled =
      abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);

  auto cleanup = on_scope_exit([&] { std::free(demangled); });

  if (status == -1) {
    throw std::bad_alloc();
  }

  // Trying to demangle an invalid name is a programming error
  assert(status == 0);

  return std::string(demangled);
#else
  return mangled;
#endif
}
}  // namespace compileus