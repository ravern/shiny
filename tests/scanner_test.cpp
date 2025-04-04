#include "frontend/scanner.h"

#include <gtest/gtest.h>

void assertScannedTokensEqual(const std::string& source,
                              const std::vector<TokenType>& expected) {
  Scanner scanner(source);
  auto tokens = scanner.scanAll();

  if (tokens.size() != expected.size()) {
    std::ostringstream actualStream;
    for (const auto& token : tokens) {
      actualStream << tokenTypeToString(token.type) << "(" << token.lexeme
                   << "), ";
    }

    std::ostringstream expectedStream;
    for (const auto& type : expected) {
      expectedStream << tokenTypeToString(type) << ", ";
    }

    FAIL() << "Token count mismatch.\n"
           << "Expected: " << expected.size() << " tokens.\n"
           << "Actual:   " << tokens.size() << " tokens.\n"
           << "Expected sequence: " << expectedStream.str() << "\n"
           << "Actual sequence:   " << actualStream.str();
  }

  for (size_t i = 0; i < expected.size(); ++i) {
    if (tokens[i].type != expected[i]) {
      FAIL() << "Token mismatch at index " << i << "\n"
             << "Expected: " << tokenTypeToString(expected[i]) << "\n"
             << "Actual:   " << tokenTypeToString(tokens[i].type)
             << " (lexeme: \"" << tokens[i].lexeme
             << "\", line: " << tokens[i].line << ")";
    }
  }
}

TEST(ScannerTest, OneLine) {
  std::string source = R"(
  let x = 3 + 2
  )";

  std::vector<TokenType> expected = {TOKEN_LET, TOKEN_IDENTIFIER, TOKEN_EQUAL,
                                     TOKEN_INT, TOKEN_PLUS,       TOKEN_INT,
                                     TOKEN_EOF};

  assertScannedTokensEqual(source, expected);
}

TEST(ScannerTest, TwoLines) {
  std::string source = R"(
  let x = 3 + 2
  var y = x - x
  )";

  std::vector<TokenType> expected = {
      TOKEN_LET,   TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT,
      TOKEN_PLUS,  TOKEN_INT,        TOKEN_VAR,   TOKEN_IDENTIFIER,
      TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_MINUS, TOKEN_IDENTIFIER,
      TOKEN_EOF};

  assertScannedTokensEqual(source, expected);
}

TEST(ScannerTest, TwoStatementsSameLine) {
  std::string source = R"(
  let x = 3 + 2 var y = x - x
  )";

  std::vector<TokenType> expected = {
      TOKEN_LET,   TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT,
      TOKEN_PLUS,  TOKEN_INT,        TOKEN_VAR,   TOKEN_IDENTIFIER,
      TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_MINUS, TOKEN_IDENTIFIER,
      TOKEN_EOF};

  assertScannedTokensEqual(source, expected);
}