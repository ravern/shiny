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
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_COLON,
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
  TOKEN_FUN,
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

inline const char* tokenTypeToString(TokenType type) {
  switch (type) {
    case TOKEN_LEFT_PAREN:
      return "LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:
      return "RIGHT_PAREN";
    case TOKEN_LEFT_BRACE:
      return "LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:
      return "RIGHT_BRACE";
    case TOKEN_COMMA:
      return "COMMA";
    case TOKEN_DOT:
      return "DOT";
    case TOKEN_MINUS:
      return "MINUS";
    case TOKEN_PLUS:
      return "PLUS";
    case TOKEN_SEMICOLON:
      return "SEMICOLON";
    case TOKEN_SLASH:
      return "SLASH";
    case TOKEN_STAR:
      return "STAR";
    case TOKEN_COLON:
      return "COLON";

    case TOKEN_BANG:
      return "BANG";
    case TOKEN_BANG_EQUAL:
      return "BANG_EQUAL";
    case TOKEN_EQUAL:
      return "EQUAL";
    case TOKEN_EQUAL_EQUAL:
      return "EQUAL_EQUAL";
    case TOKEN_GREATER:
      return "GREATER";
    case TOKEN_GREATER_EQUAL:
      return "GREATER_EQUAL";
    case TOKEN_LESS:
      return "LESS";
    case TOKEN_LESS_EQUAL:
      return "LESS_EQUAL";

    case TOKEN_IDENTIFIER:
      return "IDENTIFIER";
    case TOKEN_STRING:
      return "STRING";
    case TOKEN_INT:
      return "INT";
    case TOKEN_FLOAT:
      return "FLOAT";
    case TOKEN_AND:
      return "AND";
    case TOKEN_CLASS:
      return "CLASS";
    case TOKEN_ELSE:
      return "ELSE";
    case TOKEN_FALSE:
      return "FALSE";
    case TOKEN_FOR:
      return "FOR";
    case TOKEN_FUN:
      return "FUN";
    case TOKEN_IF:
      return "IF";
    case TOKEN_LET:
      return "LET";
    case TOKEN_NIL:
      return "NIL";
    case TOKEN_OR:
      return "OR";
    case TOKEN_PRINT:
      return "PRINT";
    case TOKEN_RETURN:
      return "RETURN";
    case TOKEN_SUPER:
      return "SUPER";
    case TOKEN_SELF:
      return "SELF";
    case TOKEN_TRUE:
      return "TRUE";
    case TOKEN_VAR:
      return "VAR";
    case TOKEN_WHILE:
      return "WHILE";

    case TOKEN_EOF:
      return "EOF";
    default:
      throw std::runtime_error("Unknown token type");
  }
}

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
