#include "shiny.h"

#include <readline/history.h>
#include <readline/readline.h>

#include <fstream>

#include "debug.h"
#include "frontend/ast_pretty_printer.h"
#include "frontend/compiler.h"
#include "frontend/parser.h"
#include "frontend/type_inference.h"
#include "frontend/var.h"
#include "vm/vm.h"

namespace Shiny {
class Interpreter {
  StringInterner interner;
  VM vm;

  TypeEnv inferenceGlobals = {};
  std::vector<VariableName> compilerGlobals;

 public:
  Interpreter() : vm(interner) {}

  void run(const std::string& source) {
    try {
      Scanner scanner(source);
      Parser parser(scanner, interner);
      auto ast = parser.parse();
      if (parser.hadError()) {
        return;
      }

      TypeInference inference(interner, &inferenceGlobals);
      inference.perform(*ast);

      ASTPrettyPrinter printer(interner);
      printer.print(*ast);

      Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel,
                        compilerGlobals, interner, *ast);
      auto rootFunction = ObjectPtr<FunctionObject>(compiler.compile());

      Value result = vm.evaluate(rootFunction);
      std::cout << std::endl;

      // print the result of the last statement
      std::cout << valueToString(result, interner) << std::endl;
    } catch (const Error& e) {
      std::cout << "Error: " << e.what() << std::endl;
    }
  }

  void runFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
      std::cerr << "Could not open file: " << filename << std::endl;
      exit(1);
    }
    std::string input((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    run(input);
  }

  void repl() {
    std::cout << "Shiny REPL. Type 'exit' to quit.\n";

    auto initialPrompt = ">>> ";
    auto multilinePrompt = "... ";
    auto currentPrompt = initialPrompt;

    std::string input;
    char* _line = nullptr;
    while ((_line = readline(currentPrompt)) != nullptr) {
      std::string line(_line);
      free(_line);

      if (!line.empty() && line.back() == '\\') {
        if (!input.empty()) {
          input.push_back('\n');
        }
        line.pop_back();
        input += line;
        currentPrompt = multilinePrompt;
        continue;
      }

      if (input.empty() && line == "exit") {
        break;
      }
      if (!input.empty()) {
        input.push_back('\n');
      }
      input += line;

      currentPrompt = initialPrompt;

      if (!input.empty()) {
        add_history(input.c_str());
        run(input);
        input.clear();
      }
    }
  }
};

// Public API
void run(const std::string& source) {
  Interpreter interpreter;
  interpreter.run(source);
}

void runFile(const std::string& filename) {
  Interpreter interpreter;
  interpreter.runFile(filename);
}

void repl() {
  Interpreter interpreter;
  interpreter.repl();
}
}  // namespace Shiny
