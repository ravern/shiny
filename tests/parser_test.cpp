#include "frontend/parser.h"

#include <gtest/gtest.h>

#include "frontend/ast_pretty_printer.h"
#include "frontend/scanner.h"

TEST(ParserTest, ValidProgram) {
  std::string source = R"(
  var x = 3 + 2
  var y = true
  var z = false
  var a = y && z
  a
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);

  auto x = strings.intern("x");
  auto y = strings.intern("y");
  auto z = strings.intern("z");
  auto a = strings.intern("a");
  auto ast = parser.parse();
  auto expectedAst =
      S::Block({
        S::Declare(x, E::Add(E::Int("3"), E::Int("2"))),
        S::Declare(y, E::Bool(true)),
        S::Declare(z, E::Bool(false)),
        S::Declare(a, E::And(E::Var(y), E::Var(z))),
        S::Expression(E::Var(a))
      });
  ASSERT_FALSE(parser.hadError());
  ASTPrettyPrinter printer(strings);
  printer.print(*ast);
  printer.print(*expectedAst);
  ASSERT_EQ(*expectedAst, *ast);
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

  auto add = strings.intern("add");
  auto x = strings.intern("x");
  auto y = strings.intern("y");
  auto z = strings.intern("z");
  auto ast = parser.parse();
  auto expectedAst = S::Block({
    S::Function(
      add,
      {Var(x, T::Int()), Var(y, T::Int())},
      T::Int(),
      S::Block({
        S::Declare(z, E::Add(E::Var(x), E::Var(y))),
        S::Return(E::Var(z))
      })
    )
  });

  ASSERT_FALSE(parser.hadError());
  ASTPrettyPrinter printer(strings);
  printer.print(*ast);
  printer.print(*expectedAst);
  ASSERT_EQ(*expectedAst, *ast);
}

