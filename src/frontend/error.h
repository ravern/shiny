#ifndef ERROR_H
#define ERROR_H
#include <error.h>

using namespace Shiny;

class ReferenceError : public Error {
public:
  ReferenceError(std::string message)
    : Error(message) {}
};

class SyntaxError : public Error {
public:
  SyntaxError(std::string message)
    : Error(message) {}
};

#endif //ERROR_H
