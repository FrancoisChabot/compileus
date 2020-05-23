#ifndef COMPILEUS_LEXER_H
#define COMPILEUS_LEXER_H

#include <string>
#include <string_view>
#include <vector>

namespace compileus {

struct Token {
  enum class Type {
    identifier,
    number,

    // punctuation marks
    exclam,        // !
    pound,         // #
    dollar,        // $
    percent,       // %
    amp,           // &
    parens_open,   // (
    parens_close,  // )
    star,          // *
    plus,          // +
    comma,         // ,
    minus,         // -
    period,        // .
    f_slash,       // /
    colon,         // :
    semicolon,     // ;
    lt,            // <
    eq,            // =
    gt,            // >
    question,      // ?
    at,            // @
    brack_open,    // [
    back_slash,    // '\'
    brack_close,   // ]
    caret,         // ^
    underscore,    // _
    backtick,      // `
    curly_open,    // {
    pipe,          // |
    curly_close,   // }
    tilde,         // ~

    // Compound punctuation
    amp_amp,        // &&
    plus_plus,      // ++
    minus_minus,    // --
    period_period,  // ..
    colon_colon,    // ::
    lt_lt,          // <<
    eq_eq,          // ==
    gt_gt,          // >>
    pipe_pipe,      // ||
    plus_eq,        // +=
    minus_eq,       // -=
    gt_eq,          // >=
    lt_eq,          // <=
    lt_gt,          // <>
    exclam_eq,      // !=

    end_of_stream,
  };

  Token(Type in_type, std::string_view in_raw, std::string in_str,
        std::string_view in_line);

  Token(std::string_view in_raw, int value, std::string_view in_line);

  [[nodiscard]] auto type() const -> Type;
  [[nodiscard]] bool operator==(Type type) const;
  [[nodiscard]] bool operator!=(Type type) const;
  [[nodiscard]] auto raw() const -> std::string_view;
  [[nodiscard]] auto line() const -> std::string_view;
  [[nodiscard]] auto str() const -> const std::string&;
  [[nodiscard]] auto is_end() const -> bool;

  [[nodiscard]] auto number_value() const -> int;

  [[nodiscard]] auto test_invariant() const -> bool;

 private:
  Type type_;

  // This is the raw buffer of the token itself.
  std::string_view raw_;

  // This is the token as interpreted.
  std::string str_;

  int number_value_ = 0;

  // The raw line of code[[nodiscard]] the token is on.
  std::string_view line_;
};

auto tokenize(std::string_view data) -> std::vector<Token>;
}  // namespace compileus

#endif