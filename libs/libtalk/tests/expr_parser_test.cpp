#include "compileus/libtalk/parser.h"
#include "doctest.h"

using compileus::ast::Array_literal_expr;
using compileus::ast::Binop_expr;
using compileus::ast::Id_ref_expr;
using compileus::ast::Invoke_expr;

template <typename T>
auto expect_expr(const char* code) -> std::unique_ptr<T> {
  auto tokens = compileus::tokenize(code);

  auto current = tokens.cbegin();
  auto expr = parse_expr(current);
  CHECK(current->is_end());

  std::unique_ptr<T> result;
  auto as_t = dynamic_cast<T*>(expr.get());
  if (as_t) {
    (void)expr.release();
    result.reset(as_t);
  }

  REQUIRE(!!result);

  return result;
}

TEST_CASE("basic expression") {
  expect_expr<Id_ref_expr>("abc");
}

TEST_CASE("terminated expression") {
  auto tokens = compileus::tokenize("abc; def");

  auto current = tokens.cbegin();
  auto expr = parse_expr(current);

  CHECK(current->str() == ";");

  CHECK(expr != nullptr);
}

TEST_CASE("Array literal") {
  auto expr_0 = expect_expr<Array_literal_expr>("[]");
  CHECK(expr_0->size() == 0);

  auto expr_1 = expect_expr<Array_literal_expr>("[1]");
  CHECK(expr_1->size() == 1);

  auto expr_2 = expect_expr<Array_literal_expr>("[1, 2]");
  CHECK(expr_2->size() == 2);
}

TEST_CASE("simple binop") {
  auto binop = expect_expr<compileus::ast::Binop_expr>("code_point == 9");
  CHECK(binop->type() == compileus::ast::Binop_type::eq_cmp);
}

TEST_CASE("compound left-binding binop") {
  auto root = expect_expr<Binop_expr>("a + b - c");
  CHECK(root->type() == compileus::ast::Binop_type::sub);

  auto lhs = dynamic_cast<const Binop_expr*>(&root->lhs());
  auto rhs = dynamic_cast<const Id_ref_expr*>(&root->rhs());

  REQUIRE(lhs != nullptr);
  REQUIRE(rhs != nullptr);

  CHECK(lhs->type() == compileus::ast::Binop_type::add);
  CHECK(rhs->name() == "c");

  auto lhs_lhs = dynamic_cast<const Id_ref_expr*>(&lhs->lhs());
  auto lhs_rhs = dynamic_cast<const Id_ref_expr*>(&lhs->rhs());

  REQUIRE(lhs_lhs != nullptr);
  REQUIRE(lhs_rhs != nullptr);

  CHECK(lhs_lhs->name() == "a");
  CHECK(lhs_rhs->name() == "b");
}

TEST_CASE("compound heterogenous binop") {
  auto binop = expect_expr<Binop_expr>("a == 1 || b == 2");
  CHECK(binop->type() == compileus::ast::Binop_type::log_or);
}

TEST_CASE("invoke") {
  CHECK(expect_expr<Invoke_expr>("abc()")->args().size() == 0);
  CHECK(expect_expr<Invoke_expr>("abc(a, b)")->args().size() == 2);
}
