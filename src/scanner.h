#ifndef SCANNER_H
#define SCANNER_H
#include <string>

enum TokenType {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
  // One or two character tokens.
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,
  // Literals.
  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
  // Keywords.
  TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
  TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
  TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_SELF,
  TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

  TOKEN_ERROR, TOKEN_EOF,

  TOKEN_COUNT
};

struct Token {
  TokenType type;
  std::string_view lexeme;
  int line;

  Token(const TokenType type, std::string_view lexeme, const int line) : type(type), lexeme(lexeme), line(line) {}
};

class Scanner {
  std::string_view source;
  int start;
  int current;
  int line;

public:
  explicit Scanner(std::string_view source) {
    this->source = source;
    start = 0;
    current = 0;
    line = 1;
  }

  Token scan();

private:
  Token string();
  Token number();
  Token identifier();
  TokenType identifierType();
  TokenType checkKeyword(int offset, std::string_view rest, TokenType type);
  bool isDigit(char c);
  bool isAlpha(char c);
  char advance();
  char peek();
  char peekNext();
  bool match(char expected);
  bool isAtEnd() const;
  void skipWhitespace();
  Token makeToken(TokenType type) const;
  Token errorToken(std::string_view message) const;
};

#endif //SCANNER_H
