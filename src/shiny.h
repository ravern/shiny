#pragma once

#include <string>

#include "runtime/value.h"

namespace Shiny {

Value run(const std::string& source, bool verbose = false);
Value runFile(const std::string& filename, bool verbose = false);
void repl(bool verbose = false);

}  // namespace Shiny
