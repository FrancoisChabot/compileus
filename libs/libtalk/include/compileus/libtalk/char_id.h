#ifndef COMPILEUS_CHAR_ID_H
#define COMPILEUS_CHAR_ID_H

#include <cstdint>

namespace compileus {

using char_type = std::uint32_t;

auto is_whitespace(char_type cp) -> bool;
auto is_line_break(char_type cp) -> bool;
auto is_punctuation(char_type cp) -> bool;
auto is_digit(char_type cp, int base) -> bool;
auto is_alpha(char_type cp) -> bool;
}  // namespace compileus

#endif