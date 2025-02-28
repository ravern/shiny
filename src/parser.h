#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "ast.h"
#include <stdexcept>

class Parser {
  Scanner& scanner;
  std::optional<Token> current;
  std::optional<Token> prev;

public:
  Parser(Scanner& s)
      : scanner(s), current(), prev() {}

  std::unique_ptr<Expression> parse() {
    advance();
    return expression();
  }

private:
  std::unique_ptr<Expression> expression() {
    return equality();
  }

  std::unique_ptr<Expression> equality() {
    auto expr = comparison();
    while (match(TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
      char op = prev->lexeme[0];
      auto right = comparison();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> comparison() {
    auto expr = term();
    while (match(TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL)) {
      char op = prev->lexeme[0];
      auto right = term();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> term() {
    auto expr = factor();
    while (match(TOKEN_MINUS, TOKEN_PLUS)) {
      char op = prev->lexeme[0];
      auto right = factor();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> factor() {
    auto expr = unary();
    while (match(TOKEN_SLASH, TOKEN_STAR)) {
      char op = prev->lexeme[0];
      auto right = unary();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> unary() {
    if (match(TOKEN_BANG, TOKEN_MINUS)) {
      char op = prev->lexeme[0];
      auto right = unary();
      return std::make_unique<BinaryExpr>(op, nullptr, std::move(right));
    }
    return primary();
  }

  std::unique_ptr<Expression> primary() {
    if (match(TOKEN_NUMBER)) {
      return std::make_unique<NumberExpr>(std::stoi(std::string(prev->lexeme)));
    }
    if (match(TOKEN_IDENTIFIER)) {
      return std::make_unique<VariableExpr>(std::string(prev->lexeme));
    }
    if (match(TOKEN_LEFT_PAREN)) {
      auto expr = expression();
      consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
      return expr;
    }
    throw std::runtime_error("Expect expression.");
  }

  void advance() {
    prev = current;
    current = scanner.scan();
  }

  bool match(TokenType type) {
    if (current->type == type) {
      advance();
      return true;
    }
    return false;
  }

  bool match(TokenType type1, TokenType type2) {
    return match(type1) || match(type2);
  }

  bool match(TokenType type1, TokenType type2, TokenType type3, TokenType type4) {
    return match(type1) || match(type2) || match(type3) || match(type4);
  }

  void consume(TokenType type, const std::string& message) {
    if (current->type == type) {
      advance();
      return;
    }
    throw std::runtime_error(message);
  }
};

#endif // PARSER_H
