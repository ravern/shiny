#pragma once

#include <stdexcept>

namespace Shiny {

// Custom runtime error class for Shiny. Just makes it easier to handle errors
// that specifically come from Shiny.
class Error : public std::runtime_error {
 public:
  Error(const std::string& message) : std::runtime_error(message) {}
};

}  // namespace Shiny