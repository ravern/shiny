#include <argparse/argparse.hpp>
#include <iostream>
#include <boost/spirit/include/qi.hpp>

// #include "bytecode.h"
#include "vm/vm.h"

namespace qi = boost::spirit::qi;

// Define a simple grammar for parsing arithmetic expressions
template <typename Iterator>
struct SimpleParser : qi::grammar<Iterator, int(), qi::space_type>
{
    SimpleParser() : SimpleParser::base_type(expression)
    {
        expression = term >> *(('+' >> term) | ('-' >> term));
        term = qi::int_;
    }

    qi::rule<Iterator, int(), qi::space_type> expression, term;
};

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

int main2(int argc, char** argv)
{
    std::string input;
    std::cout << "Enter an arithmetic expression (e.g., 1 + 2 - 3): ";
    std::getline(std::cin, input);

    SimpleParser<std::string::const_iterator> parser;
    std::string::const_iterator iter = input.begin(), end = input.end();
    int result;

    bool success = qi::phrase_parse(iter, end, parser, qi::space, result);

    if (success && iter == end)
    {
        std::cout << "Parsing succeeded! Result: " << result << std::endl;
    }
    else
    {
        std::cerr << "Parsing failed at: " << std::string(iter, end) << std::endl;
    }

    return 0;
}
