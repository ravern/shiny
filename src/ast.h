#ifndef AST_H
#define AST_H

#include <vector>

class ExpressionStmt;
class AssignmentStmt;
class NumberExpr;
class VariableExpr;
class BinaryExpr;

class ExpressionVisitor {
public:
  virtual ~ExpressionVisitor() = default;
  virtual void visitNumberExpr(const NumberExpr& expr) const = 0;
  virtual void visitVariableExpr(const VariableExpr& expr) const = 0;
  virtual void visitBinaryExpr(const BinaryExpr& expr) const = 0;
};

class Expression {
  public:
  virtual ~Expression() = default;
  virtual void accept(const ExpressionVisitor& visitor) const = 0;
};

class NumberExpr : public Expression {
public:
  std::string_view literal;

  explicit NumberExpr(std::string_view literal) : literal(literal) {}

  void accept(const ExpressionVisitor& visitor) const override {
    return visitor.visitNumberExpr(*this);
  }
};

class VariableExpr : public Expression {
  public:
  std::string_view name;

  explicit VariableExpr(std::string_view name) : name(name) {}

  void accept(const ExpressionVisitor& visitor) const override {
    return visitor.visitVariableExpr(*this);
  }
};

class BinaryExpr : public Expression {
  public:
  char op;
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;

  BinaryExpr(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
  : op(op), left(std::move(left)), right(std::move(right)) {}

  void accept(const ExpressionVisitor& visitor) const override {
    return visitor.visitBinaryExpr(*this);
  }
};

class StatementVisitor {
public:
  virtual ~StatementVisitor() = default;
  virtual void visitAssignmentStmt(const AssignmentStmt& stmt) const = 0;
  virtual void visitExpressionStmt(const ExpressionStmt& stmt) const = 0;
};

class Statement {
public:
  virtual ~Statement() = default;
  virtual void accept(const StatementVisitor& visitor) const = 0;
};

class AssignmentStmt : public Statement {
 public:
  std::string_view varName;
  std::optional<std::string_view> typeAnnotation;
  std::unique_ptr<Expression> expr;

  AssignmentStmt(const std::string_view var_name,
                 const std::optional<std::string_view> type_annotation,
                 std::unique_ptr<Expression> expr)
      : varName(var_name), typeAnnotation(type_annotation), expr(std::move(expr)) {}

  void accept(const StatementVisitor& visitor) const override {
    return visitor.visitAssignmentStmt(*this);
  }
};

class ExpressionStmt : public Statement {
public:
  std::unique_ptr<Expression> expr;

  explicit ExpressionStmt(std::unique_ptr<Expression> expr)
  : expr(std::move(expr)) {}

  void accept(const StatementVisitor& visitor) const override {
    return visitor.visitExpressionStmt(*this);
  }
};

class Program {
 public:
  std::vector<std::unique_ptr<Statement>> statements;

  explicit Program(std::vector<std::unique_ptr<Statement>> statements)
      : statements(std::move(statements)) {}
};

#endif // AST_H
