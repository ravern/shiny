#include <argparse/argparse.hpp>

#include "shiny.h"

int main(int argc, char** argv) {
  argparse::ArgumentParser program("shiny");
  program.add_argument("-V", "--verbose")
      .help("include output helpful for debugging")
      .default_value(false)
      .implicit_value(true);
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
    Shiny::runFile(program.get<std::string>("file"),
                   program.get<bool>("verbose"));
  } else {
    Shiny::repl(program.get<bool>("verbose"));
  }
  return 0;
}