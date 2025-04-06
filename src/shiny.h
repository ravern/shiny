#ifndef SHINY_H
#define SHINY_H

#include <string>

namespace Shiny {
  void run(const std::string& source);
  void runFile(const std::string& filename);
  void repl();
}

#endif //SHINY_H
