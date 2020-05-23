#include "compileus/libtalk/lexer.h"
#include "doctest.h"

void check_token_sequence(const char* code,
                          const std::vector<std::string>& expected) {
  auto tokens = compileus::tokenize(code);

  REQUIRE(tokens.size() == expected.size() + 1);

  for (std::size_t i = 0; i < tokens.size() - 1; ++i) {
    CHECK(tokens[i].str() == expected[i]);
  }

  CHECK(tokens.back().is_end());
}

TEST_CASE("Simple complete function") {
  check_token_sequence(
      "let is_whitespace(code_point) = code_point == 9 || code_point == 32;",
      {"let", "is_whitespace", "(", "code_point", ")", "=", "code_point",
       "==", "9", "||", "code_point", "==", "32", ";"});
}

TEST_CASE("Punctuation checks") {
  check_token_sequence("+-", {"+", "-"});
}

TEST_CASE("various whitespace") {
  check_token_sequence("allo", {"allo"});
  check_token_sequence(" allo", {"allo"});
  check_token_sequence("  allo", {"allo"});
  check_token_sequence("  al lo ", {"al", "lo"});
}

TEST_CASE("number_literal") {
  auto check_number = [](const char* code, int value) {
    auto tokens = compileus::tokenize(code);
    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens[0].type() == compileus::Token::Type::number);
    CHECK(tokens[0].number_value() == value);
  };

  check_number("0", 0);
  check_number("12", 12);
  check_number("0xa", 0xa);
  check_number("0xAb", 0xAb);
}