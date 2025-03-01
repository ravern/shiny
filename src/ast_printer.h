#ifndef PRINTER_H
#define PRINTER_H
#include "ast.h"

class ASTPrinter : public ExpressionVisitor, public StatementVisitor {
public:
  void visitNumberExpr(const NumberExpr& expr) const override {
    std::cout << expr.literal;
  }

  void visitVariableExpr(const VariableExpr& expr) const override {
    std::cout << expr.name;
  }

  void visitBinaryExpr(const BinaryExpr& expr) const override {
    std::cout << "(";
    expr.left->accept(*this);
    std::cout << " " << expr.op << " ";
    expr.right->accept(*this);
    std::cout << ")";
  }

  void visitAssignmentStmt(const AssignmentStmt& stmt) const override {
    std::cout << "var " << stmt.varName;
    if (stmt.typeAnnotation) {
      std::cout << ": " << *stmt.typeAnnotation;
    }
    std::cout << " = ";
    stmt.expr->accept(*this);
    std::cout << ";\n";
  }

  void visitExpressionStmt(const ExpressionStmt& stmt) const override {
    stmt.expr->accept(*this);
    std::cout << ";\n";
  }
};

#endif //PRINTER_H
