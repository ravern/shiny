#include "frontend/compiler.h"
#include "frontend/factory.h"
#include "frontend/string_interner.h"
#include "frontend/type_inference.h"
#include "frontend/var.h"
#include "runtime/value.h"
#include "vm/vm.h"

int main() {
  StringInterner interner;

  SymbolId fooId = interner.intern("foo");
  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");
  SymbolId zId = interner.intern("z");

  auto ast = S::Block(
      {S::Function(fooId, {Var(xId, T::Int()), Var(yId, T::Int())}, T::Int(),
                   S::Block({S::Return(E::Sub(E::Var(xId), E::Var(yId)))})),
       S::Declare(zId, E::Apply(E::Var(fooId), {E::Int("3"), E::Int("4")}))});

  TypeInference inference(interner);
  inference.perform(*ast);

  Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto rootFunction = ObjectPtr<FunctionObject>(compiler.compile());

  VM vm;
  auto result = vm.evaluate(rootFunction);
  std::cout << result.asInt() << std::endl;

  return 0;
}