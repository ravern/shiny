#ifndef PARSER_H
#define PARSER_H

#include "factory.h"
#include "scanner.h"
#include "stmt.h"

class ParseError : public Error {
public:
  std::string_view lexeme;
  int line;

  explicit ParseError(std::string_view lexeme, int line, const std::string& message)
    : Error(message), lexeme(lexeme), line(line) {}

  void print(std::string_view source) {
    // Extract the full line of source where the error occurred.
    // First, find the beginning of the line.
    size_t lineStart = lexeme.data() - source.data();
    while (lineStart > 0 && source[lineStart - 1] != '\n') {
      --lineStart;
    }

    // Then, find the end of the line.
    size_t lineEnd = lexeme.data() - source.data() + lexeme.size();
    while (lineEnd < source.size() && source[lineEnd] != '\n') {
      ++lineEnd;
    }

    std::string_view lineText = source.substr(lineStart, lineEnd - lineStart);

    // Compute the column (character offset from lineStart to token start)
    size_t tokenStartColumn = lexeme.data() - (source.data() + lineStart);

    // Print the error
    std::cerr << "[line " << line << "] Error";
    if (line == -1) {
      std::cerr << " at end";
    } else {
      std::cerr << " at '" << lexeme << "'";
    }
    std::cerr << ": " << what() << "\n";

    std::cerr << "    " << lineText << "\n";
    std::cerr << "    " << std::string(tokenStartColumn, ' ') << "^\n";
  }
};

class Parser {
  Scanner& scanner;
  StringInterner& strings;
  Token current;
  Token previous;

 public:
  std::vector<ParseError> errors;

  explicit Parser(Scanner& scanner, StringInterner& strings)
      : scanner(scanner), strings(strings) {}

  std::unique_ptr<BlockStmt> parse() {
    advance();
    return program();
  }

  bool hadError() {
    return !errors.empty();
  }

 private:
  std::unique_ptr<BlockStmt> program() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (current.type != TOKEN_EOF) {
      statements.push_back(statement());
    }
    return S::Block(std::move(statements));
  }

  std::unique_ptr<BlockStmt> block() {
    consume(TOKEN_LEFT_BRACE, "Expected '{'");
    std::vector<std::unique_ptr<Stmt>> statements;
    while (current.type != TOKEN_RIGHT_BRACE && current.type != TOKEN_EOF) {
      statements.push_back(statement());
    }
    consume(TOKEN_RIGHT_BRACE, "Expected '}'");
    return S::Block(std::move(statements));
  }

  std::unique_ptr<Stmt> statement() {
    try {
      if (!current.isAtStartOfLine) {
        throw errorAtCurrent("Statement must begin on a new line");
      }
      if (match(TOKEN_VAR)) {
        return declareStatement();
      }
      if (match(TOKEN_RETURN)) {
        return returnStatement();
      }
      if (match(TOKEN_FUNC)) {
        return functionStatement();
      }
      return expressionStatement();
    } catch (const ParseError& e) {
      synchronize();
      return nullptr;
    }
  }

  std::unique_ptr<Stmt> declareStatement() {
    auto identifier = consume(TOKEN_IDENTIFIER, "Expected identifier");
    consume(TOKEN_EQUAL, "Expected '='");
    auto expr = expression();

    auto symbol = strings.intern(std::string(identifier.lexeme));
    return S::Declare(symbol, std::move(expr));
  }

  std::unique_ptr<Stmt> returnStatement() {
    auto expr = expression();
    return S::Return(std::move(expr));
  }

  std::unique_ptr<Stmt> functionStatement() {
    auto identifier = consume(TOKEN_IDENTIFIER, "Expected identifier");
    auto symbol = strings.intern(std::string(identifier.lexeme));
    consume(TOKEN_LEFT_PAREN, "Expected '('");

    std::vector<Var> params;
    if (!check(TOKEN_RIGHT_PAREN)) {
      do {
        auto paramName = consume(TOKEN_IDENTIFIER, "Expected identifier");
        auto paramSymbol = strings.intern(std::string(paramName.lexeme));
        consume(TOKEN_COLON, "Expected ':'");
        auto paramType = type();
        Var param(paramSymbol, paramType);
        params.push_back(param);
      } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expected ')'");

    std::shared_ptr<Type> returnType = T::Void();
    if (match(TOKEN_ARROW)) {
      returnType = type();
    }

    auto body = block();

    return S::Function(symbol, params, returnType, std::move(body));
  }

  std::unique_ptr<Stmt> expressionStatement() {
    auto expr = expression();
    return S::Expression(std::move(expr));
  }

  std::unique_ptr<Expr> expression() { return logicalOr(); }

  std::unique_ptr<Expr> logicalOr() {
    auto expr = logicalAnd();
    while (match(TOKEN_OR)) {
      auto rhs = factor();
      expr = E::Binary(std::move(expr), BinaryOperator::Or, std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> logicalAnd() {
    auto expr = equality();
    while (match(TOKEN_AND)) {
      auto rhs = factor();
      expr = E::Binary(std::move(expr), BinaryOperator::And, std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> equality() {
    auto expr = comparison();
    while (match(TOKEN_EQUAL_EQUAL) || match(TOKEN_BANG_EQUAL)) {
      BinaryOperator op =
          previous.type == TOKEN_EQUAL_EQUAL ? BinaryOperator::Eq
        : previous.type == TOKEN_BANG_EQUAL ? BinaryOperator::Neq
        : throw std::runtime_error("Unexpected TokenType");
      auto rhs = comparison();
      expr = E::Binary(std::move(expr), op, std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> comparison() {
    auto expr = term();
    while (match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL) ||
           match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL)) {
      BinaryOperator op =
          previous.type == TOKEN_LESS          ? BinaryOperator::Lt
        : previous.type == TOKEN_LESS_EQUAL    ? BinaryOperator::Lte
        : previous.type == TOKEN_GREATER       ? BinaryOperator::Gt
        : previous.type == TOKEN_GREATER_EQUAL ? BinaryOperator::Gte
        : throw std::runtime_error("Unexpected TokenType");
      auto rhs = term();
      expr = E::Binary(std::move(expr), op, std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> term() {
    auto expr = factor();
    while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) {
      BinaryOperator op =
          previous.type == TOKEN_PLUS ? BinaryOperator::Add
        : previous.type == TOKEN_MINUS ? BinaryOperator::Minus
        : throw std::runtime_error("Unexpected TokenType");
      auto rhs = factor();
      expr = E::Binary(std::move(expr), op, std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> factor() {
    auto expr = unary();
    while (match(TOKEN_STAR) || match(TOKEN_SLASH) || match(TOKEN_PERCENT)) {
      BinaryOperator op =
          previous.type == TOKEN_STAR    ? BinaryOperator::Multiply
        : previous.type == TOKEN_SLASH   ? BinaryOperator::Divide
        : previous.type == TOKEN_PERCENT ? BinaryOperator::Modulo
        : throw std::runtime_error("Unexpected TokenType");

      auto rhs = unary();
      expr = E::Binary(std::move(expr), op, std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> unary() {
    if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
      UnaryOperator op =
          previous.type == TOKEN_BANG ? UnaryOperator::Not
        : previous.type == TOKEN_MINUS ? UnaryOperator::Negate
        : throw std::runtime_error("Unexpected TokenType");
      auto rhs = unary();
      return E::Unary(op, std::move(rhs));
    }
    return call();
  }

  std::unique_ptr<Expr> call() {
    auto expr = primary();
    while (true) {
      if (match(TOKEN_LEFT_PAREN)) {
        expr = finishCall(std::move(expr));
      } else {
        break;
      }
    }
    return expr;
  }

  std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> args;
    if (!check(TOKEN_RIGHT_PAREN)) {
      do {
        if (args.size() == 255) {
          errorAtCurrent("Can't have more than 255 parameters.");
        }
        args.push_back(expression());
      } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expected ')'");
    return E::Apply(std::move(callee), std::move(args));
  }

  std::unique_ptr<Expr> primary() {
    if (match(TOKEN_INT)) {
      return E::Int(previous.lexeme);
    }
    if (match(TOKEN_FLOAT)) {
      return E::Double(previous.lexeme);
    }
    if (match(TOKEN_TRUE)) {
      return E::Bool(true);
    }
    if (match(TOKEN_FALSE)) {
      return E::Bool(false);
    }
    if (match(TOKEN_IDENTIFIER)) {
      auto symbol = strings.intern(std::string(previous.lexeme));
      return E::Var(symbol);
    }
    if (match(TOKEN_LEFT_PAREN)) {
      auto expr = expression();
      consume(TOKEN_RIGHT_PAREN, "Expected ')'");
      return expr;
    }
    throw errorAtCurrent("Expected expression");
  }

  std::shared_ptr<Type> type() {
    if (match(TOKEN_IDENTIFIER)) {
      std::string_view name = previous.lexeme;
      if (name == "Int") return T::Int();
      if (name == "Double") return T::Double();
      if (name == "Bool") return T::Bool();
      throw errorAtPrevious("Cannot find type '" + std::string(name) + "' in scope");
    }

    if (match(TOKEN_LEFT_PAREN)) {
      if (match(TOKEN_RIGHT_PAREN)) {
        return T::Void();
      }

      // Try parsing one or more types as parameters
      std::vector<std::shared_ptr<Type>> parameters;
      do {
        parameters.push_back(type());
      } while (match(TOKEN_COMMA));
      consume(TOKEN_RIGHT_PAREN, "Expected ')'");

      // Look ahead: is this a function type?
      if (match(TOKEN_ARROW)) {
        auto returnType = type();
        return T::Function(parameters, returnType);
      }

      // Is this a grouped type? e.g. (Int)
      if (parameters.size() == 1) {
        return parameters[0];
      }
    }
    throw errorAtCurrent("Expected type");
  }

  void advance() {
    previous = current;
    try {
      current = scanner.scan();
    } catch (ScanError& e) {
      throw errorAt(e.lexeme, e.line, e.what());
    }
  }

  Token& consume(TokenType type, std::string message) {
    if (current.type != type) {
      throw errorAtCurrent(message);
    }
    advance();
    return previous;
  }

  bool check(TokenType type) { return current.type == type; }

  bool match(TokenType type) {
    if (!check(type)) {
      return false;
    }
    advance();
    return true;
  }

  ParseError errorAtPrevious(std::string message) {
    return errorAt(previous, message);
  }

  ParseError errorAtCurrent(std::string message) {
    return errorAt(current, message);
  }

  ParseError errorAt(Token& token, std::string message) {
    return errorAt(token.lexeme, token.type == TOKEN_EOF ? -1 : token.line, message);
  }

  ParseError errorAt(std::string_view lexeme, int line, std::string message) {
    ParseError error(lexeme, line, message);
    error.print(scanner.source);
    errors.emplace_back(error);
    throw error;
  }

  void synchronize() {
    advance();
    while (current.type != TOKEN_EOF) {
      if (current.isAtStartOfLine) {
        return;
      }
      switch (current.type) {
        case TOKEN_VAR:
          return;
        default:
          advance();
      }
    }
  }
};

#endif  // PARSER_H
