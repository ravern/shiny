#include <gtest/gtest.h>

#include <string>
#include <unordered_map>

#include "shiny.h"

class E2ETest : public ::testing::Test {
 protected:
  std::unordered_map<std::string, Value> testCases = {
      {"adder.swift", Value(static_cast<int64_t>(3))},
      {"assign.swift", Value(static_cast<int64_t>(2))},
      {"assign_in_func.swift", Value(static_cast<int64_t>(2))},
      {"assign_in_closure.swift", Value(static_cast<int64_t>(2))},
      {"factorial.swift", Value(static_cast<int64_t>(120))},
      {"boolean.swift", Value::TRUE},
      {"nested_func.swift", Value(static_cast<int64_t>(25))},
      {"double.swift", Value(3.14159)},
      {"objects.swift", Value(static_cast<int64_t>(8))},
      {"simple_function.swift", Value(static_cast<int64_t>(1000))},
      {"ifs.swift", Value(static_cast<int64_t>(4))}};
};

// Run each test case
TEST_F(E2ETest, RunAllTests) {
  for (const auto& [filename, expectedResult] : testCases) {
    std::string filepath = "tests/e2e/" + filename;
    Value result = Shiny::runFile(filepath);

    if (result.isInt()) {
      EXPECT_TRUE(expectedResult.isInt());
      EXPECT_EQ(result.asInt(), expectedResult.asInt())
          << "Failed on file: " << filepath;
    } else if (result.isDouble()) {
      EXPECT_TRUE(expectedResult.isDouble());
      EXPECT_DOUBLE_EQ(result.asDouble(), expectedResult.asDouble())
          << "Failed on file: " << filepath;
    } else if (result.isBool()) {
      EXPECT_TRUE(expectedResult.isBool());
      EXPECT_EQ(result.asBool(), expectedResult.asBool())
          << "Failed on file: " << filepath;
    } else {
      FAIL() << "Failed on file: " << filepath;
    }
  }
}
