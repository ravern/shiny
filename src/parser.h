#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "ast.h"
#include <stdexcept>

class Parser {
  Scanner& scanner;
  std::optional<Token> next;
  std::optional<Token> current;

public:
  Parser(Scanner& s)
      : scanner(s), next(), current() {}

  std::unique_ptr<Program> parse() {
    advance();
    return program();
  }

private:
  std::unique_ptr<Program> program() {
    std::vector<std::unique_ptr<Statement>> statements = {};
    while (!isAtEnd()) {
      statements.push_back(statement());
    }
    return std::make_unique<Program>(std::move(statements));
  }

  std::unique_ptr<Statement> statement() {
    if (match(TOKEN_VAR)) {
      return assignmentStatement();
    }
    return expressionStatement();
  }

  std::unique_ptr<Statement> assignmentStatement() {
    if (!match(TOKEN_IDENTIFIER)) {
      throw std::runtime_error("Expected identifier.");
    }
    auto identifier = current;

    std::optional<Token> typeAnnotation;
    if (match(TOKEN_COLON)) {
      if (!match(TOKEN_IDENTIFIER)) {
        throw std::runtime_error("Expected type identifier.");
      }
      typeAnnotation = current;
    }

    if (!match(TOKEN_EQUAL)) {
      throw std::runtime_error("Expected =.");
    }

    auto expr = expression();
    return std::make_unique<AssignmentStmt>(
      identifier->lexeme,
      typeAnnotation.transform([](const Token& token) {
          return token.lexeme;
      }),
      std::move(expr)
    );
  }

  std::unique_ptr<Statement> expressionStatement() {
    auto expr = expression();
    return std::make_unique<ExpressionStmt>(std::move(expr));
  }

  std::unique_ptr<Expression> expression() {
    return equality();
  }

  std::unique_ptr<Expression> equality() {
    auto expr = comparison();
    while (match(TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
      char op = current->lexeme[0];
      auto right = comparison();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> comparison() {
    auto expr = term();
    while (match(TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL)) {
      char op = current->lexeme[0];
      auto right = term();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> term() {
    auto expr = factor();
    while (match(TOKEN_MINUS, TOKEN_PLUS)) {
      char op = current->lexeme[0];
      auto right = factor();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> factor() {
    auto expr = unary();
    while (match(TOKEN_SLASH, TOKEN_STAR)) {
      char op = current->lexeme[0];
      auto right = unary();
      expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expression> unary() {
    if (match(TOKEN_BANG, TOKEN_MINUS)) {
      char op = current->lexeme[0];
      auto right = unary();
      return std::make_unique<BinaryExpr>(op, nullptr, std::move(right));
    }
    return primary();
  }

  std::unique_ptr<Expression> primary() {
    if (match(TOKEN_NUMBER)) {
      return std::make_unique<NumberExpr>(current->lexeme);
    }
    if (match(TOKEN_IDENTIFIER)) {
      return std::make_unique<VariableExpr>(current->lexeme);
    }
    if (match(TOKEN_LEFT_PAREN)) {
      auto expr = expression();
      consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
      return expr;
    }
    throw std::runtime_error("Expected expression.");
  }

  void advance() {
    current = next;
    next = scanner.scan();
  }

  bool match(TokenType type) {
    if (next->type == type) {
      advance();
      return true;
    }
    return false;
  }

  template <typename... TokenTypes>
  bool match(TokenTypes... types) {
    TokenType typeArray[] = {types...};  // Convert pack into array
    for (TokenType type : typeArray) {
      if (match(type)) return true;  // Short-circuit
    }
    return false;
  }

  void consume(TokenType type, const std::string& message) {
    if (next->type == type) {
      advance();
      return;
    }
    throw std::runtime_error(message);
  }

  bool isAtEnd() {
    return next->type == TOKEN_EOF;
  }
};

#endif // PARSER_H
