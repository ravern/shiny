#ifndef FACTORY_H
#define FACTORY_H

#include "expr.h"
#include "stmt.h"
#include <vector>

#include "var.h"

// Factory functions for expressions
namespace E {
  // Literal expressions
  inline std::shared_ptr<IntegerExpr> Int(std::string_view literal) {
    return std::make_shared<IntegerExpr>(literal);
  }

  inline std::shared_ptr<DoubleExpr> Double(std::string_view literal) {
    return std::make_shared<DoubleExpr>(literal);
  }

  inline std::shared_ptr<BoolExpr> Bool(bool value) {
    return std::make_shared<BoolExpr>(value);
  }

  // Variable expressions
  inline std::shared_ptr<VariableExpr> Var(::Var var) {
    return std::make_shared<VariableExpr>(var);
  }

  inline std::shared_ptr<VariableExpr> Var(VariableName name) {
    return std::make_shared<VariableExpr>(::Var(name));
  }

  inline std::shared_ptr<VariableExpr> Var(VariableName name,
                                           std::shared_ptr<Type> type) {
    return std::make_shared<VariableExpr>(::Var(name, std::move(type)));
  }

  // Binary expressions
  inline std::shared_ptr<BinaryExpr> Binary(std::shared_ptr<Expr> left,
                                            BinaryOperator op,
                                            std::shared_ptr<Expr> right) {
    return std::make_shared<BinaryExpr>(std::move(left), op, std::move(right));
  }

  // Shorthand for common binary operations
  inline std::shared_ptr<BinaryExpr> Add(std::shared_ptr<Expr> left,
                                         std::shared_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::Add, std::move(right));
  }

  inline std::shared_ptr<BinaryExpr> Sub(std::shared_ptr<Expr> left,
                                         std::shared_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::Minus, std::move(right));
  }

  inline std::shared_ptr<BinaryExpr> And(std::shared_ptr<Expr> left,
                                         std::shared_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::And, std::move(right));
  }

  inline std::shared_ptr<BinaryExpr> Or(std::shared_ptr<Expr> left,
                                        std::shared_ptr<Expr> right) {
    return Binary(std::move(left), BinaryOperator::Or, std::move(right));
  }

  // Unary expressions
  inline std::shared_ptr<UnaryExpr> Unary(UnaryOperator op,
                                          std::shared_ptr<Expr> operand) {
    return std::make_shared<UnaryExpr>(op, std::move(operand));
  }

  inline std::shared_ptr<UnaryExpr> Not(std::shared_ptr<Expr> operand) {
    return Unary(UnaryOperator::Not, std::move(operand));
  }

  inline std::shared_ptr<UnaryExpr> Neg(std::shared_ptr<Expr> operand) {
    return Unary(UnaryOperator::Negate, std::move(operand));
  }

  // Function application
  inline std::shared_ptr<ApplyExpr>
  Apply(std::shared_ptr<Expr> function, std::vector<std::shared_ptr<Expr> > args) {
    return std::make_shared<ApplyExpr>(std::move(function), std::move(args));
  }
} // namespace E

// Factory functions for statements
namespace S {
  // Block statement
  inline std::shared_ptr<BlockStmt>
  Block(std::vector<std::shared_ptr<Stmt> > statements) {
    return std::make_shared<BlockStmt>(std::move(statements));
  }

  // Declare statement
  inline std::shared_ptr<DeclareStmt> Declare(VariableName name,
                                              std::shared_ptr<Expr> initializer) {
    return std::make_shared<DeclareStmt>(::Var(name), std::move(initializer));
  }

  inline std::shared_ptr<DeclareStmt> Declare(::Var var,
                                              std::shared_ptr<Expr> initializer) {
    return std::make_shared<DeclareStmt>(std::move(var), std::move(initializer));
  }

  // Assign statement
  inline std::shared_ptr<AssignStmt> Assign(VariableName name,
                                            std::shared_ptr<Expr> value) {
    return std::make_shared<AssignStmt>(::Var(name), std::move(value));
  }

  // If statement
  inline std::shared_ptr<IfStmt> If(std::shared_ptr<Expr> condition,
                                    std::shared_ptr<Stmt> thenBranch,
                                    std::shared_ptr<Stmt> elseBranch) {
    return std::make_shared<IfStmt>(std::move(condition), std::move(thenBranch),
                                    std::move(elseBranch));
  }

  // Function statement
  inline std::shared_ptr<FunctionStmt> Function(VariableName name,
                                                std::vector<::Var> params,
                                                std::shared_ptr<Type> returnType,
                                                std::shared_ptr<BlockStmt> body) {
    return std::make_shared<FunctionStmt>(::Var(name), std::move(params),
                                          std::move(returnType), std::move(body));
  }

  // Return statement
  inline std::shared_ptr<ReturnStmt> Return(std::shared_ptr<Expr> value) {
    return std::make_shared<ReturnStmt>(std::move(value));
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
} // namespace T

#endif // FACTORY_H
