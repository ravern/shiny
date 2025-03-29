#include "compiler.h"

#include <gtest/gtest.h>

#include "frontend/factory.h"
#include "frontend/type.h"

TEST(CompilerTest, Test) {
  StringInterner interner;
  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");
  SymbolId zId = interner.intern("z");
  auto ast = S::Block({
    S::Declare(xId, E::Int("3")),
    S::Declare(yId, E::Int("4")),
    S::Declare(zId, E::Add(E::Var(xId), E::Var(yId)))
  });

  auto compiler = Compiler(nullptr, Compiler::FunctionKind::Script, interner, *ast);
  auto result = compiler.compile();
  std::cout << "Done" << std::endl;
}

