#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
  // Single-character tokens.
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_COLON,
  TOKEN_PERCENT,
  // One or two character tokens.
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_BITWISE_AND,
  TOKEN_AND,
  TOKEN_BITWISE_OR,
  TOKEN_OR,
  TOKEN_ARROW,
  // Literals.
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_INT,
  TOKEN_FLOAT,
  // Keywords.
  TOKEN_CLASS,
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FOR,
  TOKEN_FUNC,
  TOKEN_IF,
  TOKEN_LET,
  TOKEN_NIL,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_SUPER,
  TOKEN_SELF,
  TOKEN_TRUE,
  TOKEN_VAR,
  TOKEN_WHILE,
  // Misc
  TOKEN_EOF,
};

struct Token {
  TokenType type;
  std::string_view lexeme;
  int line{};
  bool isAtStartOfLine;

  Token(TokenType type, const std::string_view lexeme, int line,
        const bool is_at_start_of_line)
      : type(type),
        lexeme(lexeme),
        line(line),
        isAtStartOfLine(is_at_start_of_line) {}

  Token() : type(TOKEN_EOF), line(-1), isAtStartOfLine(false) {};
};

#endif  // TOKEN_H
