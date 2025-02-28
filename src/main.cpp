#include <iostream>
#include <fstream>
#include "parser.h"
#include "scanner.h"
#include <argparse/argparse.hpp>

void run_repl() {
  std::cout << "Shiny REPL. Type 'exit' to quit.\n";
  std::string input;
  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, input) || input == "exit") break;

    try {
      Scanner scanner(input);
      Parser parser(scanner);
      auto expr = parser.parse();
      expr->print();
      std::cout << std::endl;
    } catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << std::endl;
    }
  }
}

void run_file(const std::string& filename) {
  std::ifstream file(filename);
  if (!file) {
    std::cerr << "Could not open file: " << filename << std::endl;
    exit(1);
  }

  std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  try {
    Scanner scanner(input);
    Parser parser(scanner);
    auto expr = parser.parse();
    expr->print();
    std::cout << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    exit(1);
  }
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
    run_file(program.get<std::string>("file"));
  } else {
    run_repl();
  }
  return 0;
}
