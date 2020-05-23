#include "compileus/libtalk/lexer.h"

#include <cassert>
#include <map>
#include <set>
#include <stdexcept>

#include "compileus/libtalk/char_id.h"

using std::size_t;
using std::string;
using std::string_view;

namespace compileus {

Token::Token(Type in_type, std::string_view in_raw, std::string in_str,
             std::string_view in_line)
    : type_(in_type), raw_(in_raw), str_(std::move(in_str)), line_(in_line) {}

Token::Token(std::string_view in_raw, int value, std::string_view in_line)
    : type_(Type::number),
      raw_(in_raw),
      str_(in_raw),
      number_value_(value),
      line_(in_line) {}

auto Token::type() const -> Type {
  return type_;
}

[[nodiscard]] bool Token::operator==(Type type) const {
  return type == type_;
}

[[nodiscard]] bool Token::operator!=(Type type) const {
  return type != type_;
}

auto Token::raw() const -> std::string_view {
  return raw_;
}

auto Token::line() const -> std::string_view {
  return line_;
}

auto Token::number_value() const -> int {
  return number_value_;
}

auto Token::str() const -> const std::string& {
  return str_;
}

auto Token::is_end() const -> bool {
  return type_ == Type::end_of_stream;
}

[[nodiscard]] auto Token::test_invariant() const -> bool {
  if (type_ == Type::end_of_stream) {
    return raw_.empty() && str_.empty() && line_.empty();
  }

  return raw_.data() >= line_.data() &&
         raw_.data() - line_.data() < line_.size() &&
         raw_.data() - line_.data() + raw_.size() < line_.size();
}

namespace {

template <typename ITE>
auto tokenize_punctuation(ITE& ite, ITE end, std::string_view line) -> Token {
  // Important!
  // tokenize_punctuation() assumes that all multi-char punctuation are just a
  // single-char extension of a valid punctuation.
  static const std::map<std::string_view, Token::Type> punctuation_symbols = {
      {"!", Token::Type::exclam},       {"#", Token::Type::pound},
      {"$", Token::Type::dollar},       {"%", Token::Type::percent},
      {"&", Token::Type::amp},          {"(", Token::Type::parens_open},
      {")", Token::Type::parens_close}, {"*", Token::Type::star},
      {"+", Token::Type::plus},         {",", Token::Type::comma},
      {"-", Token::Type::minus},        {".", Token::Type::period},
      {"/", Token::Type::f_slash},      {":", Token::Type::colon},
      {";", Token::Type::semicolon},    {"<", Token::Type::lt},
      {"=", Token::Type::eq},           {">", Token::Type::gt},
      {"?", Token::Type::question},     {"@", Token::Type::at},
      {"[", Token::Type::brack_open},   {"\\", Token::Type::back_slash},
      {"]", Token::Type::brack_close},  {"^", Token::Type::caret},
      {"_", Token::Type::underscore},   {"`", Token::Type::backtick},
      {"{", Token::Type::curly_open},   {"|", Token::Type::pipe},
      {"}", Token::Type::curly_close},  {"~", Token::Type::tilde},
      {"&&", Token::Type::amp_amp},     {"++", Token::Type::plus_plus},
      {"--", Token::Type::minus_minus}, {"..", Token::Type::period_period},
      {"::", Token::Type::colon_colon}, {"<<", Token::Type::lt_lt},
      {"==", Token::Type::eq_eq},       {">>", Token::Type::gt_gt},
      {"||", Token::Type::pipe_pipe},   {"+=", Token::Type::plus_eq},
      {"-=", Token::Type::minus_eq},    {">=", Token::Type::gt_eq},
      {"<=", Token::Type::lt_eq},       {"<>", Token::Type::lt_gt},
      {"!=", Token::Type::exclam_eq},
  };

  assert(ite != end);
  assert(is_punctuation(*ite));

  auto start = ite;
  size_t i = 1;
  string_view raw{&*start, i};
  ++ite;

  Token::Type type = punctuation_symbols.at(raw);

  while (ite != end) {
    string_view next = string_view{&*start, ++i};

    auto found = punctuation_symbols.find(next);
    if (found != punctuation_symbols.end()) {
      ++ite, raw = next;
      type = found->second;
    } else {
      break;
    }
  }

  return Token{type, raw, string{raw}, line};
}

template <typename ITE>
auto tokenize_number(ITE& ite, ITE end, std::string_view line) -> Token {
  assert(ite != end);
  assert(is_digit(*ite, 10));

  auto start = ite++;
  int base = 10;
  if (ite != end && *start == '0' && (*ite == 'x' || *ite == 'X')) {
    base = 16;
    ++ite;
  }

  while (ite != end && is_digit(*ite, base)) {
    ++ite;
  }

  string_view raw{&*start, size_t(std::distance(start, ite))};
  int v = std::stoi(std::string(raw), nullptr, base);

  return {raw, v, line};
}

template <typename ITE>
auto tokenize_identifier(ITE& ite, ITE end, std::string_view line) -> Token {
  assert(ite != end);
  assert(is_alpha(*ite));

  auto start = ite++;

  while (ite != end && (is_alpha(*ite) || is_digit(*ite, 10))) {
    ++ite;
  }

  string_view raw{&*start, size_t(std::distance(start, ite))};
  return {Token::Type::identifier, raw, string(raw), line};
}

template <typename ITE>
auto get_line_of_code(ITE ite, ITE end) -> std::string_view {
  assert(ite != end);

  const char* start = &*ite;
  auto ptr = std::next(ite);

  while (ptr != end && !is_line_break(*ptr)) {
    ++ptr;
  }

  return std::string_view{start, std::size_t(std::distance(ite, ptr))};
}
}  // namespace

auto tokenize(string_view data) -> std::vector<Token> {
  std::vector<Token> result;

  auto ite = data.cbegin();
  auto end = data.cend();

  auto line = get_line_of_code(ite, end);

  while (ite != end) {
    constexpr int last_bit = 0x80;
    if ((*ite & last_bit) != 0) {
      throw std::domain_error("Only ASCII data is supported at the moment.");
    }

    // skip leading whitespace
    if (is_whitespace(*ite)) {
      bool lb = is_line_break(*ite);
      ++ite;

      if (lb && ite != end && is_line_break(*ite)) {
        line = get_line_of_code(ite, end);
      }

      continue;
    }

    if (is_punctuation(*ite)) {
      result.push_back(tokenize_punctuation(ite, end, line));
    } else if (is_digit(*ite, 10)) {
      result.push_back(tokenize_number(ite, end, line));
    } else if (is_alpha(*ite)) {
      result.push_back(tokenize_identifier(ite, end, line));
    } else {
      throw std::domain_error("Invalid character");
    }
  }

  result.emplace_back(Token{Token::Type::end_of_stream, "", "", line});

  return result;
}
}  // namespace compileus