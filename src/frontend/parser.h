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

  std::shared_ptr<BlockStmt> parse() {
    advance();
    return program();
  }

  bool hadError() {
    return !errors.empty();
  }

 private:
  std::shared_ptr<BlockStmt> program() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (current.type != TOKEN_EOF) {
      statements.push_back(statement());
    }
    return S::Block(statements);
  }

  std::shared_ptr<BlockStmt> block() {
    consume(TOKEN_LEFT_BRACE, "Expected '{'");
    std::vector<std::shared_ptr<Stmt>> statements;
    while (current.type != TOKEN_RIGHT_BRACE && current.type != TOKEN_EOF) {
      statements.push_back(statement());
    }
    consume(TOKEN_RIGHT_BRACE, "Expected '}'");
    return S::Block(statements);
  }

  std::shared_ptr<Stmt> statement() {
    try {
      if (!current.isAtStartOfLine) {
        throw errorAtCurrent("Statement must begin on a new line");
      }
      if (match(TOKEN_VAR)) {
        return declareStatement();
      }
      if (match(TOKEN_FUNC)) {
        return functionStatement();
      }
      throw errorAtCurrent("Expected statement");
    } catch (const ParseError& e) {
      synchronize();
      return nullptr;
    }
  }

  std::shared_ptr<Stmt> declareStatement() {
    auto identifier = consume(TOKEN_IDENTIFIER, "Expected identifier");
    consume(TOKEN_EQUAL, "Expected '='");
    auto expr = expression();

    auto symbol = strings.intern(std::string(identifier.lexeme));
    return S::Declare(symbol, expr);
  }

  std::shared_ptr<Stmt> functionStatement() {
    auto identifier = consume(TOKEN_IDENTIFIER, "Expected identifier");
    auto symbol = strings.intern(std::string(identifier.lexeme));
    consume(TOKEN_LEFT_PAREN, "Expected '('");

    std::vector<Var> params;
    if (!check(TOKEN_RIGHT_PAREN)) {
      do {
        auto paramName = consume(TOKEN_IDENTIFIER, "Expected identifier");
        consume(TOKEN_COLON, "Expected ':'");
        auto paramType = consume(TOKEN_IDENTIFIER, "Expected type for function parameter");
        auto paramSymbol = strings.intern(std::string(paramName.lexeme));
        auto type = parseType(paramType.lexeme);
        Var param(paramSymbol, type);
        params.push_back(param);
      } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expected ')'");

    std::shared_ptr<Type> returnType = T::Void();
    if (match(TOKEN_ARROW)) {
      auto argType = consume(TOKEN_IDENTIFIER, "Expected type for function result");
      returnType = parseType(argType.lexeme);
    }

    auto body = block();

    return S::Function(symbol, params, returnType, body);
  }

  std::shared_ptr<Type> parseType(std::string_view str) {
    std::shared_ptr<Type> type =
       str == "Int" ? std::static_pointer_cast<Type>(T::Int())
     : str == "Double" ? std::static_pointer_cast<Type>(T::Double())
     : str == "Bool" ? std::static_pointer_cast<Type>(T::Bool())
     : throw std::runtime_error("Unexpected type");
    return type;
  }

  std::shared_ptr<Expr> expression() { return logicalOr(); }

  std::shared_ptr<Expr> logicalOr() {
    auto expr = logicalAnd();
    while (match(TOKEN_OR)) {
      auto rhs = factor();
      expr = E::Binary(expr, BinaryOperator::Or, rhs);
    }
    return expr;
  }

  std::shared_ptr<Expr> logicalAnd() {
    auto expr = equality();
    while (match(TOKEN_AND)) {
      auto rhs = factor();
      expr = E::Binary(expr, BinaryOperator::And, rhs);
    }
    return expr;
  }

  std::shared_ptr<Expr> equality() { return comparison(); }

  std::shared_ptr<Expr> comparison() { return term(); }

  std::shared_ptr<Expr> term() {
    auto expr = factor();
    while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) {
      BinaryOperator op =
          previous.type == TOKEN_PLUS ? BinaryOperator::Add
        : previous.type == TOKEN_MINUS ? BinaryOperator::Minus
        : throw std::runtime_error("Unexpected token type");
      auto rhs = factor();
      expr = E::Binary(expr, op, rhs);
    }
    return expr;
  }

  std::shared_ptr<Expr> factor() { return unary(); }

  std::shared_ptr<Expr> unary() {
    if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
      UnaryOperator op =
          previous.type == TOKEN_BANG ? UnaryOperator::Not
        : previous.type == TOKEN_MINUS ? UnaryOperator::Negate
        : throw std::runtime_error("Unexpected token type");
      auto rhs = unary();
      return E::Unary(op, rhs);
    }
    return primary();
  }

  std::shared_ptr<Expr> primary() {
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
