#include "frontend/ast_pretty_printer.h"
#include "frontend/compiler.h"
#include "frontend/parser.h"
#include "frontend/scanner.h"
#include "frontend/string_interner.h"
#include "frontend/type_inference.h"
#include "runtime/value.h"
#include "vm/vm.h"

int main() {
  StringInterner interner;

  std::string source = R"(
  func foo(x: Int, y: Int) -> Int {
    var z = x + y
    return x + y - z
  }
  var z = foo(3, 4)
  )";

  Scanner scanner(source);
  Parser parser(scanner, interner);
  auto ast = parser.parse();
  if (parser.hadError()) {
    return 1;
  }

  TypeInference inference(interner);
  inference.perform(*ast);

  ASTPrettyPrinter printer(interner);
  printer.print(*ast);

  Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel, interner, *ast);
  auto rootFunction = ObjectPtr<FunctionObject>(compiler.compile());

  VM vm;
  vm.evaluate(rootFunction);

  return 0;
}