#include <gtest/gtest.h>

#include "frontend/union_find.h"
#include "frontend/factory.h"

TEST(UnionFindTest, InsertAndFind) {
  UnionFind uf;
  TypeVar var1 = uf.insert(std::nullopt);
  TypeVar var2 = uf.insert(std::nullopt);
  EXPECT_EQ(uf.find(var1), var1);
  EXPECT_EQ(uf.find(var2), var2);
}

TEST(UnionFindTest, UnionSimple) {
  UnionFind uf;
  TypeVar var1 = uf.insert(std::nullopt);
  TypeVar var2 = uf.insert(std::nullopt);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
}

TEST(UnionFindTest, UnionWithTypes) {
  UnionFind uf;
  auto type1 = T::Int();
  auto type2 = T::Int();
  TypeVar var1 = uf.insert(type1);
  TypeVar var2 = uf.insert(type2);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_TRUE(uf.getType(var2).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *type1);
}

TEST(UnionFindTest, UnionDifferentTypes) {
  UnionFind uf;
  auto type1 = T::Int();
  auto type2 = T::Var(1);
  TypeVar var1 = uf.insert(type1);
  TypeVar var2 = uf.insert(type2);
  EXPECT_THROW(uf.join(var1, var2), UnificationError);
}

TEST(UnionFindTest, MergeIntWithNullopt) {
  UnionFind uf;
  auto type1 = T::Int();
  TypeVar var1 = uf.insert(type1);
  TypeVar var2 = uf.insert(std::nullopt);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *type1);
}

TEST(UnionFindTest, MergeNulloptWithInt) {
  UnionFind uf;
  TypeVar var1 = uf.insert(std::nullopt);
  auto type2 = T::Int();
  TypeVar var2 = uf.insert(type2);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *type2);
}

TEST(UnionFindTest, MergeDoubleWithNullopt) {
  UnionFind uf;
  auto type1 = T::Double();
  TypeVar var1 = uf.insert(type1);
  TypeVar var2 = uf.insert(std::nullopt);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *type1);
}

TEST(UnionFindTest, MergeNulloptWithDouble) {
  UnionFind uf;
  TypeVar var1 = uf.insert(std::nullopt);
  auto type2 = T::Double();
  TypeVar var2 = uf.insert(type2);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *type2);
}

TEST(UnionFindTest, MergeIntWithDouble) { // no subtyping :(
  UnionFind uf;
  auto type1 = T::Int();
  auto type2 = T::Double();
  TypeVar var1 = uf.insert(type1);
  TypeVar var2 = uf.insert(type2);
  EXPECT_THROW(uf.join(var1, var2), UnificationError);
}

TEST(UnionFindTest, MergeEquivalentFunctions) {
  UnionFind uf;
  auto func1 = T::Function(
      { T::Var(1) },
      T::Var(2));
  auto func2 = T::Function(
      { T::Var(1) },
      T::Var(2));
  TypeVar var1 = uf.insert(func1);
  TypeVar var2 = uf.insert(func2);
  uf.join(var1, var2);
  EXPECT_EQ(uf.find(var1), uf.find(var2));
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *func1);
}

TEST(UnionFindTest, MergeDifferentFunctions) {
  UnionFind uf;
  auto func1 = T::Function(
      { T::Var(1) },
      T::Var(2));
  auto func2 = T::Function(
      { T::Var(3) },
      T::Var(4));
  TypeVar var1 = uf.insert(func1);
  TypeVar var2 = uf.insert(func2);
  EXPECT_THROW(uf.join(var1, var2), UnificationError);
}

TEST(UnionFindTest, SetTypeNullToType) {
  UnionFind uf;
  TypeVar var = uf.insert(std::nullopt);
  auto intType = T::Int();

  uf.setType(var, intType);

  EXPECT_TRUE(uf.getType(var).has_value());
  EXPECT_EQ(*uf.getType(var).value(), *intType);
}

TEST(UnionFindTest, SetTypeWithSameType) {
  UnionFind uf;
  auto intType1 = T::Int();
  TypeVar var = uf.insert(intType1);
  auto intType2 = T::Int();

  uf.setType(var, intType2);

  EXPECT_TRUE(uf.getType(var).has_value());
  EXPECT_EQ(*uf.getType(var).value(), *intType2);
}

TEST(UnionFindTest, SetTypeWithDifferentType) {
  UnionFind uf;
  auto intType = T::Int();
  TypeVar var = uf.insert(intType);
  auto varType = T::Var(1);

  EXPECT_THROW(uf.setType(var, varType), UnificationError);
}

TEST(UnionFindTest, SetTypeToNull) {
  UnionFind uf;
  auto intType = T::Int();
  TypeVar var = uf.insert(intType);

  // Setting to null should not change the type
  EXPECT_TRUE(uf.getType(var).has_value());
  EXPECT_EQ(*uf.getType(var).value(), *intType);
}

TEST(UnionFindTest, SetTypeAfterJoin) {
  UnionFind uf;
  TypeVar var1 = uf.insert(std::nullopt);
  TypeVar var2 = uf.insert(std::nullopt);

  uf.join(var1, var2);

  auto intType = T::Int();
  uf.setType(var2, intType);

  // Both variables should have the same type
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_TRUE(uf.getType(var2).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *intType);
  EXPECT_EQ(*uf.getType(var2).value(), *intType);
}

TEST(UnionFindTest, SetTypeConflictingTypes) {
  UnionFind uf;
  auto intType = T::Int();
  TypeVar var1 = uf.insert(intType);
  TypeVar var2 = uf.insert(std::nullopt);

  uf.join(var1, var2);

  auto varType = T::Var(1);
  EXPECT_THROW(uf.setType(var2, varType), UnificationError);

  // The type should remain unchanged
  EXPECT_TRUE(uf.getType(var1).has_value());
  EXPECT_TRUE(uf.getType(var2).has_value());
  EXPECT_EQ(*uf.getType(var1).value(), *intType);
  EXPECT_EQ(*uf.getType(var2).value(), *intType);
}

TEST(UnionFindTest, SetTypeComplexFunctions) {
  UnionFind uf;
  auto func1 = T::Function(
      { T::Var(1) },
      T::Var(2));
  TypeVar var = uf.insert(std::nullopt);

  uf.setType(var, func1);

  auto func2 = T::Function(
      { T::Var(1) },
      T::Var(2));

  // Setting to an equivalent function type should work
  uf.setType(var, func2);

  EXPECT_TRUE(uf.getType(var).has_value());
  EXPECT_EQ(*uf.getType(var).value(), *func2);

  auto func3 = T::Function(
      { T::Var(3) },
      T::Var(4));

  // Setting to a different function type should fail
  EXPECT_THROW(uf.setType(var, func3), UnificationError);
}
