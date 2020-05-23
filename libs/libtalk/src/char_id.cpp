#include "compileus/libtalk/char_id.h"

#include <array>

namespace {
enum Basic_char_type {
  nothi,
  digit,
  punct,
  alpha,
};

// clang-format off
  const std::array basic_chars = {
    //     x0     x1     x2     x3     x4     x5     x6     x7     x8     x9     xA     xB     xC     xD     xE     xF
    /*0x*/ nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi,
    /*1x*/ nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi,
    /*2x*/ nothi, punct, punct, punct, punct, punct, punct, punct, punct, punct, punct, punct, punct, punct, punct, punct,
    /*3x*/ digit, digit, digit, digit, digit, digit, digit, digit, digit, digit, punct, punct, punct, punct, punct, punct,
    /*4x*/ punct, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
    /*5x*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, punct, punct, punct, punct, alpha,
    /*6x*/ punct, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
    /*7x*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, punct, punct, punct, punct, nothi,
    /*8x*/ nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi,
    /*9x*/ nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi, nothi,
    /*Ax*/ nothi, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, nothi, alpha, alpha,
    /*Bx*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
    /*Cx*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
    /*Dx*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
    /*Ex*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
    /*Fx*/ alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha,
  };
// clang-format on
}  // namespace

namespace compileus {
auto is_whitespace(char_type cp) -> bool {
  const std::array ws_chars = {
      0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x0020, 0x0085, 0x00A0, 0x1680,
      0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008,
      0x2009, 0x200A, 0x2028, 0x2029, 0x202F, 0x205F, 0x3000};

  auto found = std::lower_bound(std::begin(ws_chars), std::end(ws_chars), cp);
  return found != std::end(ws_chars) && *found == cp;
}

auto is_line_break(char_type cp) -> bool {
  const std::array lb_chars = {0x000A, 0x000B, 0x000C, 0x000D,
                               0x0085, 0x2028, 0x2029};

  auto found = std::lower_bound(std::begin(lb_chars), std::end(lb_chars), cp);
  return found != std::end(lb_chars) && *found == cp;
}

auto is_punctuation(char_type cp) -> bool {
  return cp < basic_chars.size() && basic_chars[cp] == Basic_char_type::punct;
}

auto is_digit(char_type cp, int base) -> bool {
  if (cp >= basic_chars.size()) {
    return false;
  }

  int digit_seq = std::min(base, 10);
  if (cp >= '0' && cp < '0' + digit_seq) {
    return true;
  }

  if (base > 10) {
    int alpha_seq = base - 10;
    if (cp >= 'a' && cp < 'a' + alpha_seq) {
      return true;
    }
    if (cp >= 'A' && cp < 'A' + alpha_seq) {
      return true;
    }
  }

  return false;
  return cp < basic_chars.size() && basic_chars[cp] == Basic_char_type::digit;
}

auto is_alpha(char_type cp) -> bool {
  return cp >= basic_chars.size() || basic_chars[cp] == Basic_char_type::alpha;
}
}  // namespace compileus