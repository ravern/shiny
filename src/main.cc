#include <argparse/argparse.hpp>
#include <iostream>

#include "bytecode.h"
#include "vm/vm.h"

int main(int argc, char** argv) {
  argparse::ArgumentParser program("shiny");

  program.add_argument("square")
      .help("display the square of a given integer")
      .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  auto input = program.get<int>("square");
  std::cout << (input * input) << std::endl;
  return 0;
}