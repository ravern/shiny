#include "frontend/parser.h"

#include <gtest/gtest.h>

#include "frontend/scanner.h"

TEST(ParserTest, Test) {
  std::string source = R"(
  var x = 3 + 2
  var y = true
  var z = false
  var a = y && z
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
      });
  ASSERT_EQ(*expectedAst, *ast);
}

TEST(ParserTest, Invalid) {
  std::string source = R"(
  |||||var x = 1 var y = true
  )";
  Scanner scanner(source);
  StringInterner strings;
  Parser parser(scanner, strings);
  parser.parse();
}

