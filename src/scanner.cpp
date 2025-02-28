#include "scanner.h"

Token Scanner::scan() {
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
    case '"': return string();
  }

  return errorToken("Unexpected character.");
}

Token Scanner::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line++;
    advance();
  }

  if (isAtEnd()) return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
}

Token Scanner::number() {
  while (isDigit(peek())) advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek())) advance();
  }

  return makeToken(TOKEN_NUMBER);
}

Token Scanner::identifier() {
  while (isAlpha(peek()) || isDigit(peek())) advance();
  return makeToken(identifierType());
}

TokenType Scanner::identifierType() {
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
    case 's': return checkKeyword(1, "uper", TOKEN_SUPER);
    case 't':
      if (current - start > 1) {
        switch (source.at(start+1)) {
          case 'h': return checkKeyword(2, "is", TOKEN_SELF);
          case 'r': return checkKeyword(2, "ue", TOKEN_TRUE);
        }
      }
      break;
    case 'v': return checkKeyword(1, "ar", TOKEN_VAR);
    case 'w': return checkKeyword(1, "hile", TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(int offset, std::string_view rest, TokenType type) {
  if (current - start - offset == rest.size()) {
    std::string_view sourceRest = source.substr(start + offset, current - start - offset);
    if (sourceRest == rest) {
      return type;
    }
  }
  return TOKEN_IDENTIFIER;
}

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
       (c >= 'A' && c <= 'Z') ||
        c == '_';
}

char Scanner::advance() {
  return source.at(current++);
}

char Scanner::peek() {
  if (isAtEnd()) return '\0';
  return source.at(current);
}

char Scanner::peekNext() {
  if (isAtEnd()) return '\0';
  return source.at(current + 1);
}

bool Scanner::match(char expected) {
  if (isAtEnd()) return false;
  if (source.at(current) != expected) return false;
  current++;
  return true;
}

bool Scanner::isAtEnd() const {
  return current == source.size();
}

void Scanner::skipWhitespace() {
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

Token Scanner::makeToken(TokenType type) const {
  std::string_view lexeme = source.substr(start, current - start);
  return Token(type, lexeme, line);
}

Token Scanner::errorToken(std::string_view message) const {
  Token token(TOKEN_ERROR, message, line);
  return token;
}
