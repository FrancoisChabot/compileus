#ifndef COMPILEUS_PARSER_H
#define COMPILEUS_PARSER_H

#include <optional>
#include <stdexcept>

#include "compileus/libtalk/ast.h"
#include "compileus/libtalk/lexer.h"

namespace compileus {

class Parse_error : public std::runtime_error {
 public:
  Parse_error(const std::string& err, Token location);

  [[nodiscard]] auto get_location_string() const -> std::string;

 private:
  Token location_;
};

using Token_iterator = std::vector<Token>::const_iterator;

auto parse_module(Token_iterator& begin) -> ast::Module;
auto parse_declaration(Token_iterator& begin) -> ast::Decl;
auto parse_expr(Token_iterator& begin) -> std::unique_ptr<ast::Expr>;

}  // namespace compileus

#endif