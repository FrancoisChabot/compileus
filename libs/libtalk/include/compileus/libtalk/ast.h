#ifndef COMPILEUS_AST_H
#define COMPILEUS_AST_H

#include <memory>
#include <string>
#include <vector>

#include "compileus/libtalk/lexer.h"

namespace compileus {
namespace ast {

using Identifier = std::string;

class Statement {
 public:
  virtual ~Statement() = default;
};

class Expr {
 public:
  virtual ~Expr() = default;
};

class Id_ref_expr : public Expr {
 public:
  explicit Id_ref_expr(const Token& tok);

  [[nodiscard]] auto name() const -> const Identifier&;

 private:
  Identifier name_;
};

class Invoke_arg {
 public:
  explicit Invoke_arg(std::unique_ptr<Expr> value);

 private:
  std::unique_ptr<Expr> value_;
};

class Invoke_expr : public Expr {
 public:
  Invoke_expr(std::unique_ptr<Expr> lhs, std::vector<Invoke_arg> args);
  [[nodiscard]] const std::vector<Invoke_arg>& args() const;

 private:
  std::unique_ptr<Expr> lhs_;
  std::vector<Invoke_arg> args_;
};

class Number_literal_expr : public Expr {
 public:
  explicit Number_literal_expr(const Token& tok);
};

class Array_literal_expr : public Expr {
 public:
  void push_back(std::unique_ptr<Expr> expr);

  [[nodiscard]] auto size() const -> std::size_t { return members_.size(); }

 private:
  std::vector<std::unique_ptr<Expr>> members_;
};

enum class Binop_type {
  log_or,
  log_and,
  bit_or,
  bit_xor,
  bit_and,
  eq_cmp,
  neq_cmp,
  lt_cmp,
  lte_cmp,
  gt_cmp,
  gte_cmp,
  left_bshift,
  right_bshift,
  add,
  sub,
  mul,
  div,
  mod
};

enum class Unop_type {
  neg,
};

class Binop_expr : public Expr {
 public:
  Binop_expr(std::unique_ptr<Expr> lhs, Binop_type binop,
             std::unique_ptr<Expr> rhs);

  [[nodiscard]] auto type() const -> Binop_type;

  [[nodiscard]] auto lhs() const -> const Expr&;
  [[nodiscard]] auto rhs() const -> const Expr&;

  [[nodiscard]] auto test_invariant() const -> bool;

 private:
  std::unique_ptr<Expr> lhs_;
  std::unique_ptr<Expr> rhs_;
  Binop_type type_;
};

class Param {
  Identifier name_;
};

class Unop_expr : public Expr {
 public:
  Unop_expr(Unop_type unop, std::unique_ptr<Expr> rhs);

  [[nodiscard]] auto test_invariant() const -> bool;

 private:
  Unop_type unop_;
  std::unique_ptr<Expr> rhs_;
};

class Decl {
 public:
  explicit Decl(Identifier name);

  auto add_param() -> Param&;

 private:
  Identifier name_;
  std::vector<Param> parameters_;
  std::unique_ptr<Expr> expr_;
};

class Module {
 public:
  void add_decl(Decl decl);

 private:
  std::vector<Decl> decls_;
};

class Ident_ref : Expr {
  Identifier ident_;
};

}  // namespace ast
}  // namespace compileus

#endif