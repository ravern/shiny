#pragma once

#include <string>

#include "runtime/value.h"

namespace Shiny {

Value run(const std::string& source);
Value runFile(const std::string& filename);
void repl();

}  // namespace Shiny
