#ifndef FACTORY_H
#define FACTORY_H

#include "expr.h"
#include "stmt.h"
#include <vector>

#include "var.h"

// Factory functions for expressions
namespace E {
  // Literal expressions
  inline std::unique_ptr<VoidExpr> Void() {
    return std::make_unique<VoidExpr>();
  }

  inline std::unique_ptr<IntegerExpr> Int(std::string_view literal) {
    return std::make_unique<IntegerExpr>(literal);
  }

  inline std::unique_ptr<DoubleExpr> Double(std::string_view literal) {
    return std::make_unique<DoubleExpr>(literal);
  }

  inline std::unique_ptr<BoolExpr> Bool(bool value) {
    return std::make_unique<BoolExpr>(value);
  }

  // Variable expressions
  inline std::unique_ptr<VariableExpr> Var(::Var var) {
    return std::make_unique<VariableExpr>(var);
  }

  inline std::unique_ptr<VariableExpr> Var(VariableName name) {
    return std::make_unique<VariableExpr>(::Var(name));
  }

  inline std::unique_ptr<VariableExpr> Var(VariableName name,
                                           std::shared_ptr<Type> type) {
    return std::make_unique<VariableExpr>(::Var(name, std::move(type)));
  }

  // Binary expressions
  inline std::unique_ptr<BinaryExpr> Binary(std::unique_ptr<Expr> left,
                                            BinaryOperator op,
                                            std::unique_ptr<Expr> right) {
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
  }

  // Shorthand for common binary operations
  inline std::unique_ptr<BinaryExpr> Add(std::unique_ptr<Expr> left,
                                         std::unique_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::Add, std::move(right));
  }

  inline std::unique_ptr<BinaryExpr> Sub(std::unique_ptr<Expr> left,
                                         std::unique_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::Minus, std::move(right));
  }

  inline std::unique_ptr<BinaryExpr> And(std::unique_ptr<Expr> left,
                                         std::unique_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::And, std::move(right));
  }

  inline std::unique_ptr<BinaryExpr> Or(std::unique_ptr<Expr> left,
                                        std::unique_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::Or, std::move(right));
  }

  // Unary expressions
  inline std::unique_ptr<UnaryExpr> Unary(UnaryOperator op,
                                          std::unique_ptr<Expr> operand) {
    return std::make_unique<UnaryExpr>(op, std::move(operand));
  }

  inline std::unique_ptr<UnaryExpr> Not(std::unique_ptr<Expr> operand) {
    return Unary(UnaryOperator::Not, std::move(operand));
  }

  inline std::unique_ptr<UnaryExpr> Neg(std::unique_ptr<Expr> operand) {
    return Unary(UnaryOperator::Negate, std::move(operand));
  }

  // Function application
  inline std::unique_ptr<ApplyExpr>
  Apply(std::unique_ptr<Expr> function, std::vector<std::unique_ptr<Expr> > args) {
    return std::make_unique<ApplyExpr>(std::move(function), std::move(args));
  }
} // namespace E

// Factory functions for statements
namespace S {
  // Block statement
  inline std::unique_ptr<BlockStmt>
  Block(std::vector<std::unique_ptr<Stmt>> statements) {
    return std::make_unique<BlockStmt>(std::move(statements));
  }

  // Declare statement
  inline std::unique_ptr<DeclareStmt> Declare(VariableName name,
                                              std::unique_ptr<Expr> initializer) {
    return std::make_unique<DeclareStmt>(::Var(name), std::move(initializer));
  }

  inline std::unique_ptr<DeclareStmt> Declare(::Var var,
                                              std::unique_ptr<Expr> initializer) {
    return std::make_unique<DeclareStmt>(std::move(var), std::move(initializer));
  }

  // Assign statement
  inline std::unique_ptr<AssignStmt> Assign(VariableName name,
                                            std::unique_ptr<Expr> value) {
    return std::make_unique<AssignStmt>(::Var(name), std::move(value));
  }

  // Function statement
  inline std::unique_ptr<FunctionStmt> Function(VariableName name,
                                                std::vector<::Var> params,
                                                std::shared_ptr<Type> returnType,
                                                std::unique_ptr<BlockStmt> body) {
    return std::make_unique<FunctionStmt>(::Var(name), std::move(params),
                                          std::move(returnType), std::move(body));
  }

  // Expression statement
  inline std::unique_ptr<ExprStmt> Expression(std::unique_ptr<Expr> value) {
    return std::make_unique<ExprStmt>(std::move(value));
  }
} // namespace S

// Factory functions for types
namespace T {
  inline std::shared_ptr<IntegerType> Int() {
    return std::make_shared<IntegerType>();
  }

  inline std::shared_ptr<DoubleType> Double() {
    return std::make_shared<DoubleType>();
  }

  inline std::shared_ptr<BoolType> Bool() { return std::make_shared<BoolType>(); }

  inline std::shared_ptr<VoidType> Void() { return std::make_shared<VoidType>(); }

  inline std::shared_ptr<VariableType> Var(TypeVar typeVar) {
    return std::make_shared<VariableType>(typeVar);
  }

  inline std::shared_ptr<FunctionType>
  Function(std::vector<std::shared_ptr<Type> > parameters,
           std::shared_ptr<Type> returnType) {
    return std::make_shared<FunctionType>(std::move(parameters),
                                          std::move(returnType));
  }

  inline std::shared_ptr<Type> Class(
      VariableName name,
      std::unordered_map<VariableName, std::shared_ptr<Type>> fields,
      std::unordered_map<VariableName, std::shared_ptr<Type>> methods) {
    return std::make_shared<ClassType>(name, std::move(fields),
                                       std::move(methods));
  }
} // namespace T

#endif // FACTORY_H
