#include "frontend/compiler.h"
#include "frontend/factory.h"
#include "frontend/string_interner.h"
#include "vm/vm.h"

int main() {
  StringInterner interner;

  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");
  SymbolId zId = interner.intern("z");

  auto ast =
      S::Block({S::Declare(xId, E::Int("3")), S::Declare(yId, E::Int("4")),
                S::Declare(zId, E::Add(E::Var(xId), E::Var(yId)))});

  Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto rootFunction = ObjectRef(compiler.compile());

  VM vm;
  auto result = vm.evaluate(rootFunction);
  std::cout << result.toInt() << std::endl;

  return 0;
}