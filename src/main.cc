#include <argparse/argparse.hpp>
#include <fstream>

#include "frontend/ast_pretty_printer.h"
#include "frontend/compiler.h"
#include "frontend/parser.h"
#include "frontend/scanner.h"
#include "frontend/string_interner.h"
#include "frontend/type_inference.h"
#include "runtime/value.h"
#include "vm/vm.h"

StringInterner interner;
TypeInference inference(interner);
std::vector<VariableName> globals;

void interpret(const std::string& source) {
  try {
    Scanner scanner(source);
    Parser parser(scanner, interner);
    auto ast = parser.parse();
    if (parser.hadError()) {
      return;
    }

    inference.performRepl(*ast);

    ASTPrettyPrinter printer(interner);
    printer.print(*ast);

    Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel, globals, interner, *ast);
    auto rootFunction = ObjectPtr<FunctionObject>(compiler.compile());

    VM vm;
    vm.evaluate(rootFunction);
  } catch (const Error& e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

void runRepl() {
  std::cout << "Shiny REPL. Type 'exit' to quit.\n";
  std::string input;
  while (true) {
    std::cout << "> ";
    std::getline(std::cin, input);
    if (input == "exit") break;
    interpret(input);
  }
}

void runFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file) {
    std::cerr << "Could not open file: " << filename << std::endl;
    exit(1);
  }
  std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  interpret(input);
}

int main(int argc, char** argv) {
  argparse::ArgumentParser program("shiny");
  program.add_argument("file")
    .help("shiny file")
    .nargs(argparse::nargs_pattern::optional);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  if (program.present("file")) {
    runFile(program.get<std::string>("file"));
  } else {
    runRepl();
  }
  return 0;
}