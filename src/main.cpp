#include "frontend/compiler.h"
#include "frontend/factory.h"
#include "frontend/string_interner.h"
#include "frontend/type_inference.h"
#include "frontend/var.h"
#include "vm/vm.h"

int main() {
  StringInterner interner;

  SymbolId fooId = interner.intern("foo");
  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");

  auto ast =
      S::Block({S::Function(fooId, {Var(xId, T::Int())}, T::Int(),
                            S::Block({S::Return(E::Var(xId))})),
                S::Declare(yId, E::Apply(E::Var(fooId), {E::Int("3")}))});

  TypeInference inference(interner);
  inference.perform(*ast);

  Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto rootFunction = ObjectRef(compiler.compile());

  VM vm;
  auto result = vm.evaluate(rootFunction);
  std::cout << result.toDouble() << std::endl;

  return 0;
}