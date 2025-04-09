#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

#include "error.h"
#include "token.h"

using namespace Shiny;

class ScanError : public Error {
public:
  std::string_view lexeme;
  int line;

  ScanError(const std::string& message, std::string_view lexeme, int line)
    : Error(message), lexeme(lexeme), line(line) {}
};

class Scanner {
public:
  std::string_view source;
private:
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
    isAtStartOfLine = true;
  }

  std::vector<Token> scanAll() {
    std::vector<Token> tokens;
    for (;;) {
      Token token = scan();
      tokens.push_back(token);
      if (token.type == TOKEN_EOF) break;
    }
    return tokens;
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
      case '-':
        return makeToken(
          match('>') ? TOKEN_ARROW : TOKEN_MINUS);
      case '+': return makeToken(TOKEN_PLUS);
      case '/': return makeToken(TOKEN_SLASH);
      case '*': return makeToken(TOKEN_STAR);
      case '%': return makeToken(TOKEN_PERCENT);
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
      case '&':
        return makeToken(
            match('&') ? TOKEN_AND : TOKEN_BITWISE_AND);
      case '|':
        return makeToken(
            match('|') ? TOKEN_OR : TOKEN_BITWISE_OR);
      case '"':
        return string();
      default:
        throw ScanError("Unexpected character.", source.substr(current, 1), line);
    }
  }

private:
  Token string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') line++;
      advance();
    }

    if (isAtEnd()) {
      throw ScanError("Unterminated string.", source.substr(current, 1), line);
    }

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

      return makeToken(TOKEN_FLOAT);
    } else {
      return makeToken(TOKEN_INT);
    }
  }

  Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
  }

  TokenType identifierType() {
    switch (source.at(start)) {
      case 'c': return checkKeyword(1, "lass", TOKEN_CLASS);
      case 'e': return checkKeyword(1, "lse", TOKEN_ELSE);
      case 'f':
        if (current - start > 1) {
          switch (source.at(start+1)) {
            case 'a': return checkKeyword(2, "lse", TOKEN_FALSE);
            case 'o': return checkKeyword(2, "r", TOKEN_FOR);
            case 'u': return checkKeyword(2, "nc", TOKEN_FUNC);
          }
        }
        break;
      case 'i':
        if (current - start > 1) {
          switch (source.at(start+1)) {
            case 'f': return TOKEN_IF;
            case 'n': return checkKeyword(2, "it", TOKEN_INIT);
          }
        }
        break;
      case 'l': return checkKeyword(1, "et", TOKEN_LET);
      case 'n': return checkKeyword(1, "il", TOKEN_NIL);
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
      case 't': return checkKeyword(1, "rue", TOKEN_TRUE);
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
};

#endif //SCANNER_H
