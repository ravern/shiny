#include <argparse/argparse.hpp>

#include "shiny.h"

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
    Shiny::runFile(program.get<std::string>("file"));
  } else {
    Shiny::repl();
  }
  return 0;
}