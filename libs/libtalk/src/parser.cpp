#include "compileus/libtalk/parser.h"

#include <cassert>
#include <map>
#include <sstream>

namespace compileus {

namespace {
void expect(bool test, const std::string& msg, Token_iterator location) {
  if (!test) {
    throw Parse_error(msg, *location);
  }
}

void expect_not_end(Token_iterator& begin) {
  expect(!begin->is_end(), "Unexpected end.", begin);
}

// Consumes the next token and expectes it to not be the last.
void advance_no_end(Token_iterator& begin) {
  ++begin;
  expect_not_end(begin);
}

}  // namespace

Parse_error::Parse_error(const std::string& err, Token location)
    : std::runtime_error(err), location_(std::move(location)) {}

auto Parse_error::get_location_string() const -> std::string {
  if (location_.is_end()) {
    return "end of stream...";
  }

  auto tok_offset = location_.raw().data() - location_.line().data();
  auto tok_len = location_.raw().size();

  std::ostringstream str_out;

  str_out << location_.line() << "\n";

  for (int i = 0; i < tok_offset; ++i) {
    str_out << " ";
  }

  for (int i = 0; i < tok_len; ++i) {
    str_out << "^";
  }
  // Identify the start and the end of the line on which the error occured.
  return str_out.str();
}

auto parse_module(Token_iterator& begin) -> ast::Module {
  ast::Module result;

  while (!begin->is_end()) {
    if (begin->str() == "let") {
      result.add_decl(parse_declaration(begin));
    } else {
      throw Parse_error("Unexpected module-level directive.", *begin);
    }
  }

  return result;
}

// full_expr = expr ";"
//             | "{" {statement} "}"

auto parse_declaration(Token_iterator& begin) -> ast::Decl {
  // decl_id = "identifier", [ "(", {params}, ")" ]
  // declaration = "let", decl_id, "=", full_expr

  assert(!begin->is_end() && begin->str() == "let");

  // Consume the let
  advance_no_end(begin);

  //
  expect(begin->type() == Token::Type::identifier, "Expected identifier.",
         begin);
  ast::Decl result(begin->str());

  advance_no_end(begin);

  if (begin->str() == "(") {
    advance_no_end(begin);

    while (begin->str() != ")") {
      expect(begin->type() == Token::Type::identifier, "Expected identifier.",
             begin);

      advance_no_end(begin);
      auto& p = result.add_param();
    }

    // consume the closing parens
    advance_no_end(begin);
  }

  if (begin->str() == "=") {
    advance_no_end(begin);

    auto expr = parse_expr(begin);
    expect(!!expr, "Expected expression.", begin);
  } else {
    throw Parse_error("Unexpected Token.", *begin);
  }

  expect(begin->str() == ";", "Expected ;", begin);
  ++begin;
  return result;
}

auto parse_array_expr(Token_iterator& begin)
    -> std::unique_ptr<ast::Array_literal_expr>;

auto parse_expr_log_or(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_log_and(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_bit_or(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_bit_xor(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_bit_and(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_eq_cmp(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_ineq_cmp(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_bshift(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_add_sub(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_mul_div(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;

auto parse_expr_prefix(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;
auto parse_expr_postfix(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;

auto parse_primary_expr(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  expect(!begin->is_end(), "Unexpected end.", begin);

  std::unique_ptr<ast::Expr> result;

  switch (begin->type()) {
    case Token::Type::identifier:
      result = std::make_unique<ast::Id_ref_expr>(*begin);
      ++begin;
      break;
    case Token::Type::number:
      result = std::make_unique<ast::Number_literal_expr>(*begin);
      ++begin;
      break;
    case Token::Type::brack_open:
      result = parse_array_expr(begin);
      break;
    case Token::Type::parens_open:
      ++begin;
      result = parse_expr(begin);
      expect(begin->type() == Token::Type::parens_close, "Expected ).", begin);
      ++begin;
      break;
    default:
      expect(false, "Expected primary expression.", begin);
      break;
  }

  return result;
}

auto parse_expr(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  return parse_expr_log_or(begin);
}

auto parse_array_expr(Token_iterator& begin)
    -> std::unique_ptr<ast::Array_literal_expr> {
  assert(!begin->is_end());
  assert(begin->str() == "[");

  // consume the opening bracket
  advance_no_end(begin);

  auto result = std::make_unique<ast::Array_literal_expr>();

  if (begin->str() == "]") {
    ++begin;
    return result;
  }

  while (true) {
    auto expr = parse_expr(begin);
    expect(!!expr, "Expected expression", begin);

    result->push_back(std::move(expr));
    expect(!begin->is_end(), "Unexpected end.", begin);
    if (begin->str() == "]") {
      ++begin;
      break;
    }

    expect(begin->str() == ",", "Expected ,", begin);
    ++begin;
  }

  return result;
}

auto parse_expr_log_or(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_log_and(begin);

  while (!begin->is_end() && *begin == Token::Type::pipe_pipe) {
    ++begin;
    auto rhs = parse_expr_log_and(begin);
    lhs = std::make_unique<ast::Binop_expr>(
        std::move(lhs), ast::Binop_type::log_or, std::move(rhs));
  }

  return lhs;
}

auto parse_expr_log_and(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_bit_or(begin);

  while (!begin->is_end() && *begin == Token::Type::amp_amp) {
    ++begin;
    auto rhs = parse_expr_bit_or(begin);
    lhs = std::make_unique<ast::Binop_expr>(
        std::move(lhs), ast::Binop_type::log_and, std::move(rhs));
  }

  return lhs;
}

auto parse_expr_bit_or(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_bit_xor(begin);

  while (!begin->is_end() && *begin == Token::Type::pipe) {
    ++begin;
    auto rhs = parse_expr_bit_xor(begin);
    lhs = std::make_unique<ast::Binop_expr>(
        std::move(lhs), ast::Binop_type::bit_or, std::move(rhs));
  }

  return lhs;
}

auto parse_expr_bit_xor(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_bit_and(begin);

  while (!begin->is_end() && *begin == Token::Type::caret) {
    ++begin;
    auto rhs = parse_expr_bit_and(begin);
    lhs = std::make_unique<ast::Binop_expr>(
        std::move(lhs), ast::Binop_type::bit_xor, std::move(rhs));
  }

  return lhs;
}

auto parse_expr_bit_and(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_eq_cmp(begin);

  while (!begin->is_end() && *begin == Token::Type::amp_amp) {
    ++begin;
    auto rhs = parse_expr_eq_cmp(begin);
    lhs = std::make_unique<ast::Binop_expr>(
        std::move(lhs), ast::Binop_type::bit_and, std::move(rhs));
  }

  return lhs;
}

auto parse_expr_eq_cmp(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_ineq_cmp(begin);

  while (!begin->is_end()) {
    if (*begin == Token::Type::eq_eq) {
      ++begin;
      auto rhs = parse_expr_ineq_cmp(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::eq_cmp, std::move(rhs));
    } else if (*begin == Token::Type::exclam_eq) {
      ++begin;
      auto rhs = parse_expr_ineq_cmp(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::neq_cmp, std::move(rhs));
    } else {
      break;
    }
  }
  return lhs;
}

auto parse_expr_ineq_cmp(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_bshift(begin);

  while (!begin->is_end()) {
    if (*begin == Token::Type::lt) {
      ++begin;
      auto rhs = parse_expr_bshift(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::lt_cmp, std::move(rhs));
    } else if (*begin == Token::Type::gt) {
      ++begin;
      auto rhs = parse_expr_bshift(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::lte_cmp, std::move(rhs));
    } else if (*begin == Token::Type::lt_eq) {
      ++begin;
      auto rhs = parse_expr_bshift(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::gt_cmp, std::move(rhs));
    } else if (*begin == Token::Type::gt_eq) {
      ++begin;
      auto rhs = parse_expr_bshift(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::gte_cmp, std::move(rhs));
    } else {
      break;
    }
  }
  return lhs;
}

auto parse_expr_bshift(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_add_sub(begin);

  while (!begin->is_end()) {
    if (*begin == Token::Type::lt_lt) {
      ++begin;
      auto rhs = parse_expr_add_sub(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::left_bshift, std::move(rhs));
    } else if (*begin == Token::Type::gt_gt) {
      ++begin;
      auto rhs = parse_expr_add_sub(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::right_bshift, std::move(rhs));
    } else {
      break;
    }
  }
  return lhs;
}

auto parse_expr_add_sub(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_mul_div(begin);

  while (!begin->is_end()) {
    if (*begin == Token::Type::plus) {
      ++begin;
      auto rhs = parse_expr_mul_div(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::add, std::move(rhs));
    } else if (*begin == Token::Type::minus) {
      ++begin;
      auto rhs = parse_expr_mul_div(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::sub, std::move(rhs));
    } else {
      break;
    }
  }
  return lhs;
}

auto parse_expr_mul_div(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto lhs = parse_expr_prefix(begin);

  while (!begin->is_end()) {
    if (*begin == Token::Type::star) {
      ++begin;
      auto rhs = parse_expr_prefix(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::mul, std::move(rhs));
    } else if (*begin == Token::Type::f_slash) {
      ++begin;
      auto rhs = parse_expr_prefix(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::div, std::move(rhs));
    } else if (*begin == Token::Type::percent) {
      ++begin;
      auto rhs = parse_expr_prefix(begin);
      lhs = std::make_unique<ast::Binop_expr>(
          std::move(lhs), ast::Binop_type::mod, std::move(rhs));
    } else {
      break;
    }
  }
  return lhs;
}

auto parse_expr_prefix(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  expect(!begin->is_end(), "Unexpected end.", begin);

  if (*begin == Token::Type::minus) {
    ++begin;
    return std::make_unique<ast::Unop_expr>(ast::Unop_type::neg,
                                            parse_expr_prefix(begin));
  }

  return parse_expr_postfix(begin);
}

auto parse_invoke_expr(std::unique_ptr<ast::Expr> lhs, Token_iterator& begin)
    -> std::unique_ptr<ast::Expr> {
  assert(*begin == Token::Type::parens_open);
  advance_no_end(begin);

  std::vector<ast::Invoke_arg> args;

  while (*begin != Token::Type::parens_close) {
    args.emplace_back(parse_expr(begin));
    expect_not_end(begin);

    if (*begin == Token::Type::comma) {
      ++begin;
      expect_not_end(begin);
      expect(*begin != Token::Type::parens_close, "expected expression.",
             begin);
    } else {
      expect(*begin == Token::Type::parens_close, "expected either ',' or ')'.",
             begin);
    }
  }

  // consume the closing parens
  ++begin;

  return std::make_unique<ast::Invoke_expr>(std::move(lhs), std::move(args));
}

auto parse_subscript_expr(std::unique_ptr<ast::Expr> lhs, Token_iterator& begin)
    -> std::unique_ptr<ast::Expr> {
  (void)begin;
  return lhs;
}

auto parse_expr_postfix(Token_iterator& begin) -> std::unique_ptr<ast::Expr> {
  auto result = parse_primary_expr(begin);

  while (!begin->is_end()) {
    if (*begin == Token::Type::parens_open) {
      result = parse_invoke_expr(std::move(result), begin);
    } else if (*begin == Token::Type::brack_open) {
      result = parse_subscript_expr(std::move(result), begin);
    } else {
      break;
    }
  }

  return result;
}
}  // namespace compileus
