#include <gtest/gtest.h>

#include "frontend/factory.h"
#include "frontend/type.h"

TEST(TypeEqualityTest, IntegerEquality) {
  EXPECT_EQ(IntegerType(), IntegerType());
}

TEST(TypeEqualityTest, VariableEquality) {
  EXPECT_EQ(VariableType(1), VariableType(1));
}

TEST(TypeEqualityTest, VariableInequality) {
  EXPECT_NE(VariableType(1), VariableType(2));
}

TEST(TypeEqualityTest, IntegerVariableInequality) {
  EXPECT_NE(VariableType(1), IntegerType());
}

TEST(TypeEqualityTest, IntegerFunctionInequality) {
  EXPECT_NE(
    VariableType(1),
    FunctionType({ T::Var(1) }, T::Var(2))
  );
}

TEST(TypeEqualityTest, DoubleEquality) {
  EXPECT_EQ(DoubleType(), DoubleType());
}

TEST(TypeEqualityTest, DoubleIntegerInequality) {
  EXPECT_NE(DoubleType(), IntegerType());
}

TEST(TypeEqualityTest, FunctionWithDoubleEquality) {
  FunctionType func1({ T::Double() }, T::Int());
  FunctionType func2({ T::Double() }, T::Int());
  EXPECT_EQ(func1, func2);
}

TEST(TypeEqualityTest, FunctionStructuralEquality) {
  FunctionType func1({ T::Var(1) }, T::Var(2));
  FunctionType func2({ T::Var(1) }, T::Var(2));
  EXPECT_EQ(func1, func2);
}

TEST(TypeEqualityTest, FunctionReferenceEquality) {
  auto varX = T::Var(1);
  auto varY = T::Var(2);
  auto func1 = T::Function({ varX }, varY);
  auto func2 = T::Function({ varX }, varY);
  EXPECT_EQ(*func1, *func2);
}

TEST(TypeEqualityTest, NestedFunctionEquality) {
  auto nestedFunc1 = T::Function(
    { T::Function({ T::Var(1) }, T::Var(2)) },
    T::Function({ T::Var(3) }, T::Var(4))
  );
  auto nestedFunc2 = T::Function(
    { T::Function({ T::Var(1) }, T::Var(2)) },
    T::Function({ T::Var(3) }, T::Var(4))
  );
  EXPECT_EQ(*nestedFunc1, *nestedFunc2);
}
