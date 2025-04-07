#include "frontend/parser.h"

#include <gtest/gtest.h>

#include "frontend/ast_pretty_printer.h"
#include "frontend/scanner.h"

TEST(ParserTest, VarStatement) {
  std::string source = R"(
  var x = 1
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  parser.parse();
  ASSERT_FALSE(parser.hadError());
}

TEST(ParserTest, OneLineTwoStatements) {
  std::string source = R"(
  var x = 1 var y = true
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  parser.parse();
  ASSERT_EQ(parser.errors.size(), 1);
  ASSERT_NE(std::string(parser.errors[0].what()).find("Statement must begin on a new line"), std::string::npos);
}

TEST(ParserTest, VarWithoutIdentifier) {
  std::string source = R"(
  var = 1
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  parser.parse();
  ASSERT_EQ(parser.errors.size(), 1);
  ASSERT_NE(std::string(parser.errors[0].what()).find("Expected identifier"), std::string::npos);
}

TEST(ParserTest, VarWithoutInitializer) {
  std::string source = R"(
  var x =
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  parser.parse();
  ASSERT_EQ(parser.errors.size(), 1);
  ASSERT_NE(std::string(parser.errors[0].what()).find("Expected expression"), std::string::npos);
}

TEST(ParserTest, UnclosedParen) {
  std::string source = R"(
  var x = (1 + 2
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  parser.parse();
  ASSERT_EQ(parser.errors.size(), 1);
  ASSERT_NE(std::string(parser.errors[0].what()).find("Expected ')'"), std::string::npos);
}

TEST(ParserTest, FunctionWithReturnType) {
  std::string source = R"(
  func add(x: Int, y: Int) -> Int {
    var z = x + y
    return z
  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(function.params.size(), 2);
  ASSERT_EQ(*function.params[0].type.value(), *T::Int());
  ASSERT_EQ(*function.params[1].type.value(), *T::Int());
  ASSERT_EQ(*function.returnType, *T::Int());
}

TEST(ParserTest, ComplexReturnType) {
  std::string source = R"(
  func one() -> (Int) -> (Int) -> Int {
    func two(a: Int) -> ((Int) -> Int) {
      func three(b: Int) -> Int {
        return b
      }
      return three
    }
    return two
  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(
    *function.returnType,
    *T::Function(
      {T::Int()},
      T::Function({T::Int()}, T::Int())
    )
  );
}

TEST(ParserTest, ComplexReturnTypeWithParen) {
  std::string source = R"(
  func one() -> ((Int) -> (Int) -> Int) {
    func two(a: Int) -> ((Int) -> Int) {
      func three(b: Int) -> Int {
        return b
      }
      return three
    }
    return two
  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(
    *function.returnType,
    *T::Function(
      {T::Int()},
      T::Function({T::Int()}, T::Int())
    )
  );
}

TEST(ParserTest, RedundantParenReturnType) {
  std::string source = R"(
  func add(x: Int, y: Int) -> (Int) {
    return x + y
  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(
    *function.returnType,
    *T::Int()  // Redundant parens around Int shouldn't affect the AST
  );
}

TEST(ParserTest, ImplicitVoidReturnType) {
  std::string source = R"(
  func nothing() {

  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(
    *function.returnType,
    *T::Void()
  );
}

TEST(ParserTest, ExplicitVoidReturnType) {
  std::string source = R"(
  func nothing() -> () {

  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(
    *function.returnType,
    *T::Void()
  );
}

TEST(ParserTest, FunctionAsParameter) {
  std::string source = R"(
  func applyTwice(f: (Int) -> Int, x: Int) -> Int {
    return f(f(x))
  }
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  auto ast = parser.parse();
  ASSERT_FALSE(parser.hadError());

  ASSERT_EQ(ast->statements.size(), 1);
  auto& function = static_cast<FunctionStmt&>(*ast->statements[0]);
  ASSERT_EQ(function.params.size(), 2);
  ASSERT_EQ(*function.params[0].type.value(), *T::Function({T::Int()}, T::Int()));
  ASSERT_EQ(*function.params[1].type.value(), *T::Int());
  ASSERT_EQ(*function.returnType, *T::Int());
}
