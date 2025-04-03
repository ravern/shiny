#include "frontend/compiler.h"
#include "frontend/factory.h"
#include "frontend/string_interner.h"
#include "frontend/type_inference.h"
#include "vm/vm.h"

int main() {
  StringInterner interner;

  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");
  SymbolId zId = interner.intern("z");
  SymbolId wId = interner.intern("w");

  auto ast = S::Block(
      {S::Declare(xId, E::Double("3")), S::Declare(yId, E::Double("4.5")),
       S::Declare(zId, E::Double("32.5")),
       S::Declare(wId, E::Sub(E::Add(E::Var(xId), E::Var(yId)), E::Var(zId)))});

  TypeInference inference(interner);
  inference.perform(*ast);

  Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto rootFunction = ObjectRef(compiler.compile());

  VM vm;
  auto result = vm.evaluate(rootFunction);
  std::cout << result.toDouble() << std::endl;

  return 0;
}