#include "compileus/libtalk/ast.h"

#include <cassert>

namespace compileus::ast {

// ******************* Module ******************* //

void Module::add_decl(Decl decl) {
  decls_.push_back(std::move(decl));
}

// ******************* Decl ******************* //
Decl::Decl(Identifier name) : name_(std::move(name)) {}

auto Decl::add_param() -> Param& {
  parameters_.emplace_back();
  return parameters_.back();
}

// ******************* Id_ref_expr ******************* //

Id_ref_expr::Id_ref_expr(const Token& tok) : name_(tok.str()) {}

auto Id_ref_expr::name() const -> const Identifier& {
  return name_;
}

// ******************* Invoke_expr ******************* //
Invoke_arg::Invoke_arg(std::unique_ptr<Expr> value)
    : value_(std::move(value)) {}

Invoke_expr::Invoke_expr(std::unique_ptr<Expr> lhs,
                         std::vector<Invoke_arg> args)
    : lhs_(std::move(lhs)), args_(std::move(args)) {}

const std::vector<Invoke_arg>& Invoke_expr::args() const {
  return args_;
}
// ******************* Number_literal_expr ******************* //

Number_literal_expr::Number_literal_expr(const Token& /*unused*/) {}

// ******************* Array_literal_expr ******************* //

void Array_literal_expr::push_back(std::unique_ptr<Expr> expr) {
  members_.push_back(std::move(expr));
}

// ******************* Binop_expr ******************* //
auto Binop_expr::test_invariant() const -> bool {
  return lhs_ != nullptr && rhs_ != nullptr;
}

Binop_expr::Binop_expr(std::unique_ptr<Expr> lhs, Binop_type binop,
                       std::unique_ptr<Expr> rhs)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)), type_(binop) {
  assert(test_invariant());
}

auto Binop_expr::lhs() const -> const Expr& {
  assert(test_invariant());
  return *lhs_;
}

auto Binop_expr::rhs() const -> const Expr& {
  assert(test_invariant());
  return *rhs_;
}

auto Binop_expr::type() const -> Binop_type {
  return type_;
}

Unop_expr::Unop_expr(Unop_type unop, std::unique_ptr<Expr> rhs)
    : unop_(unop), rhs_(std::move(rhs)) {}

auto Unop_expr::test_invariant() const -> bool {
  return rhs_ != nullptr;
}

}  // namespace compileus::ast
