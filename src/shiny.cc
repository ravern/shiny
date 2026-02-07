#include "shiny.h"

#include <readline/history.h>
#include <readline/readline.h>

#include <fstream>

#include "built_ins.h"
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
  std::vector<Value> vmGlobals;

  bool verbose;

 public:
  Interpreter(bool verbose = false) : vm(interner, verbose), verbose(verbose) {
    // for (const auto& entry : builtIns) {
    //   VariableName name = interner.intern(entry.name);
    //   inferenceGlobals[name] = entry.type;
    //   compilerGlobals.push_back(name);
    //   vmGlobals.push_back(Value(entry.object));
    // }
  }

  Value run(const std::string& source) {
    try {
      Scanner scanner(source);
      Parser parser(scanner, interner);
      auto ast = parser.parse();
      if (parser.hadError()) {
        return Value::NIL;
      }

      TypeInference inference(interner, &inferenceGlobals);
      inference.perform(*ast);

      if (verbose) {
        ASTPrettyPrinter printer(interner);
        printer.print(*ast);
      }

      Compiler compiler(nullptr, Compiler::FunctionKind::TopLevel,
                        compilerGlobals, interner, *ast, verbose);
      auto rootFunction = ObjectPtr<FunctionObject>(compiler.compile());

      Value result = vm.evaluate(rootFunction);
      if (verbose) {
        std::cout << std::endl;
      }

      // print the result of the last statement
      std::cout << valueToString(result, interner) << std::endl;

      return result;
    } catch (const Error& e) {
      std::cout << "Error: " << e.what() << std::endl;

      return Value::NIL;
    }
  }

  Value runFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
      std::cerr << "Could not open file: " << filename << std::endl;
      exit(1);
    }
    std::string input((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    return run(input);
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
Value run(const std::string& source, bool verbose) {
  Interpreter interpreter(verbose);
  return interpreter.run(source);
}

Value runFile(const std::string& filename, bool verbose) {
  Interpreter interpreter(verbose);
  return interpreter.runFile(filename);
}

void repl(bool verbose) {
  Interpreter interpreter(verbose);
  interpreter.repl();
}

}  // namespace Shiny
