#ifndef SCANNER_H
#define SCANNER_H
#include <string>

enum TokenType {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
  TOKEN_COLON,

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
  bool isAtStartOfLine;

  Token(TokenType type, const std::string_view lexeme, int line,
        const bool is_at_start_of_line)
      : type(type),
        lexeme(lexeme),
        line(line),
        isAtStartOfLine(is_at_start_of_line) {}
};

class Scanner {
  std::string_view source;
  int start;
  int current;
  int line;
  bool isAtStartOfLine;

public:
  explicit Scanner(std::string_view source) {
    this->source = source;
    start = 0;
    current = 0;
    line = 1;
    isAtStartOfLine = false;
  }

  Token scan() {
    skipWhitespace();
    start = current;
    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
      case '(': return makeToken(TOKEN_LEFT_PAREN);
      case ')': return makeToken(TOKEN_RIGHT_PAREN);
      case '{': return makeToken(TOKEN_LEFT_BRACE);
      case '}': return makeToken(TOKEN_RIGHT_BRACE);
      case ';': return makeToken(TOKEN_SEMICOLON);
      case ':': return makeToken(TOKEN_COLON);
      case ',': return makeToken(TOKEN_COMMA);
      case '.': return makeToken(TOKEN_DOT);
      case '-': return makeToken(TOKEN_MINUS);
      case '+': return makeToken(TOKEN_PLUS);
      case '/': return makeToken(TOKEN_SLASH);
      case '*': return makeToken(TOKEN_STAR);
      case '!':
        return makeToken(
            match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
      case '=':
        return makeToken(
            match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
      case '<':
        return makeToken(
            match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
      case '>':
        return makeToken(
            match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
      case '"':
        return string();
      default:
        return errorToken("Unexpected character.");
    }
  }

private:
  Token string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') line++;
      advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_STRING);
  }

  Token number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
      // Consume the ".".
      advance();

      while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
  }

  Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
  }

  TokenType identifierType() {
    switch (source.at(start)) {
      case 'a': return checkKeyword(1, "nd", TOKEN_AND);
      case 'c': return checkKeyword(1, "lass", TOKEN_CLASS);
      case 'e': return checkKeyword(1, "lse", TOKEN_ELSE);
      case 'f':
        if (current - start > 1) {
          switch (source.at(start+1)) {
            case 'a': return checkKeyword(2, "lse", TOKEN_FALSE);
            case 'o': return checkKeyword(2, "r", TOKEN_FOR);
            case 'u': return checkKeyword(2, "n", TOKEN_FUN);
          }
        }
        break;
      case 'i': return checkKeyword(1, "f", TOKEN_IF);
      case 'n': return checkKeyword(1, "il", TOKEN_NIL);
      case 'o': return checkKeyword(1, "r", TOKEN_OR);
      case 'p': return checkKeyword(1, "rint", TOKEN_PRINT);
      case 'r': return checkKeyword(1, "eturn", TOKEN_RETURN);
      case 's':
        if (current - start > 1) {
          switch (source.at(start+1)) {
            case 'e': return checkKeyword(2, "lf", TOKEN_SELF);
            case 'u': return checkKeyword(2, "per", TOKEN_SUPER);
          }
        }
        break;
      case 't': return checkKeyword(2, "is", TOKEN_SELF);
      case 'v': return checkKeyword(1, "ar", TOKEN_VAR);
      case 'w': return checkKeyword(1, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
  }

  TokenType checkKeyword(int offset, std::string_view rest, TokenType type) {
    if (current - start - offset == rest.size()) {
      std::string_view sourceRest = source.substr(start + offset, current - start - offset);
      if (sourceRest == rest) {
        return type;
      }
    }
    return TOKEN_IDENTIFIER;
  }

  bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_';
  }

  char advance() {
    return source.at(current++);
  }

  char peek() {
    if (isAtEnd()) return '\0';
    return source.at(current);
  }

  char peekNext() {
    if (isAtEnd()) return '\0';
    return source.at(current + 1);
  }

  bool match(char expected) {
    if (isAtEnd()) return false;
    if (source.at(current) != expected) return false;
    current++;
    return true;
  }

  bool isAtEnd() const {
    return current == source.size();
  }

  void skipWhitespace() {
    for (;;) {
      char c = peek();
      switch (c) {
        case ' ':
        case '\r':
        case '\t':
          advance();
          break;
        case '\n':
          line++;
          isAtStartOfLine = true;
          advance();
          break;
        case '/':
          if (peekNext() == '/') {
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd()) advance();
          } else {
            return;
          }
          break;
        default:
          return;
      }
    }
  }

  Token makeToken(TokenType type) {
    std::string_view lexeme = source.substr(start, current - start);
    auto token = Token(type, lexeme, line, isAtStartOfLine);
    isAtStartOfLine = false;
    return token;
  }

  Token errorToken(std::string_view message) {
    Token token(TOKEN_ERROR, message, line, isAtStartOfLine);
    isAtStartOfLine = false;
    return token;
  }
};

#endif //SCANNER_H
