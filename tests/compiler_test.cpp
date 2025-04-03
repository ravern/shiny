#include "frontend/compiler.h"

#include <gtest/gtest.h>

#include "frontend/ast_pretty_printer.h"
#include "frontend/factory.h"
#include "frontend/type.h"
#include "frontend/type_inference.h"

TEST(CompilerTest, Test) {
  StringInterner interner;
  SymbolId xId = interner.intern("x");
  SymbolId yId = interner.intern("y");
  SymbolId zId = interner.intern("z");
  auto ast =
      S::Block({S::Declare(xId, E::Int("3")), S::Declare(yId, E::Int("4")),
                S::Declare(zId, E::Add(E::Var(xId), E::Var(yId)))});

  TypeInference inference = TypeInference(interner);
  inference.perform(*ast);

  ASTPrettyPrinter printer(interner);
  printer.print(*ast);

  auto compiler = Compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto result = compiler.compile();

  std::cout << "Done" << std::endl;
}

TEST(CompilerTest, Closures) {
  StringInterner interner;
  SymbolId makeAdderId = interner.intern("makeAdder");
  SymbolId aId = interner.intern("a");
  SymbolId bId = interner.intern("b");
  SymbolId addInnerId = interner.intern("addInner");
  SymbolId adderId = interner.intern("adder");
  SymbolId resultId = interner.intern("result");

  // function makeAdder(a: Int): (Int) -> Int {
  //   function addInner(b: Int): Int {
  //     return a + b;
  //   }
  //   return addInner;
  // }
  auto makeAdder = S::Function(
    makeAdderId,
    { Var(aId, T::Int()) },
    T::Function({ T::Int() }, T::Int()),
    S::Block({
      S::Function(
        addInnerId,
        { Var(bId, T::Int()) },
        T::Int(),
        S::Block({
          S::Return(E::Add(E::Var(aId), E::Var(bId)))
        })
      ),
      S::Return(E::Var(addInnerId))
    })
  );

  // let adder = makeAdder(3);
  auto adderAssign = S::Declare(
    adderId,
    E::Apply(E::Var(makeAdderId), { E::Int("3") })
  );

  // let result = adder(4); // Should evaluate to 7
  auto resultAssign = S::Declare(
    resultId,
    E::Apply(E::Var(adderId), { E::Int("4") })
  );

  auto ast = S::Block({
    makeAdder,
    adderAssign,
    resultAssign
  });

  TypeInference inference = TypeInference(interner);
  inference.perform(*ast);

  ASTPrettyPrinter printer(interner);
  printer.print(*ast);

  auto compiler = Compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto result = compiler.compile();

  std::cout << "Closure example compiled." << std::endl;
}
