#include "frontend/factory.h"

#include <iostream>

#include "frontend/expr.h"
#include "frontend/ast_pretty_printer.h"
#include "frontend/type_inference.h"

int main() {
  StringInterner interner;
  SymbolId addId = interner.intern("add");
  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");
  SymbolId resId = interner.intern("res");
  SymbolId zId = interner.intern("z");
  SymbolId aId = interner.intern("a");

  auto ast = S::Block({
    // S::Declare(
    //   zId,
    //   E::Apply(
    //     E::Var(addId),
    //     {
    //       E::Int("3"),
    //       E::Int("4")
    //     }
    //   )
    // ),
    S::Function(
      addId,
      { Var(xId, T::Int()), Var(yId, T::Int()) },
      T::Int(),
      S::Block({
        S::Declare(resId, E::Add(E::Var(xId), E::Var(yId))),
        S::Return(E::Var(4))
      })
    ),
    S::Declare(
      zId,
      E::Apply(
        E::Var(addId),
        {
          E::Int("3"),
          E::Int("4")
        }
      )
    )
  });

  TypeInference inference = TypeInference(interner);
  inference.perform(*ast);

  ASTPrettyPrinter printer(interner);
  printer.print(*ast);

  return 0;
}