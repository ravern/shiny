#include <iostream>
#include <string>
#include <vector>

class ASTNode {
  public:
  virtual ~ASTNode() = default;
  virtual void print() const = 0;  // Debugging function
};

class Expression : public ASTNode {};

class NumberExpr : public Expression {
  public:
  int value;

  explicit NumberExpr(int value) : value(value) {}

  void print() const override {
    std::cout << value;
  }
};

class VariableExpr : public Expression {
  public:
  std::string name;

  explicit VariableExpr(std::string name) : name(std::move(name)) {}

  void print() const override {
    std::cout << name;
  }
};

class BinaryExpr : public Expression {
  public:
  char op;
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;

  BinaryExpr(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
  : op(op), left(std::move(left)), right(std::move(right)) {}

  void print() const override {
    std::cout << "(";
    left->print();
    std::cout << " " << op << " ";
    right->print();
    std::cout << ")";
  }
};

class Statement : public ASTNode {};

class AssignmentStmt : public Statement {
  public:
  std::string_view varName;
  std::unique_ptr<Expression> expr;

  AssignmentStmt(std::string_view varName, std::unique_ptr<Expression> expr)
  : varName(varName), expr(std::move(expr)) {}

  void print() const override {
    std::cout << "var " << varName << " = ";
    expr->print();
    std::cout << ";\n";
  }
};

class ExpressionStmt : public Statement {
public:
  std::unique_ptr<Expression> expr;

  explicit ExpressionStmt(std::unique_ptr<Expression> expr)
  : expr(std::move(expr)) {}

  void print() const override {
    expr->print();
    std::cout << ";\n";
  }
};

class IfStmt : public Statement {
  public:
  std::unique_ptr<Expression> condition;
  std::vector<std::unique_ptr<Statement>> body;

  IfStmt(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body)
  : condition(std::move(condition)), body(std::move(body)) {}

  void print() const override {
    std::cout << "if (";
    condition->print();
    std::cout << ") {\n";
    for (const auto& stmt : body) {
      stmt->print();
    }
    std::cout << "}\n";
  }
};

class WhileStmt : public Statement {
  public:
  std::unique_ptr<Expression> condition;
  std::vector<std::unique_ptr<Statement>> body;

  WhileStmt(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body)
  : condition(std::move(condition)), body(std::move(body)) {}

  void print() const override {
    std::cout << "while (";
    condition->print();
    std::cout << ") {\n";
    for (const auto& stmt : body) {
      stmt->print();
    }
    std::cout << "}\n";
  }
};

class Program {
 public:
  std::vector<std::unique_ptr<Statement>> statements;

  explicit Program(std::vector<std::unique_ptr<Statement>> statements)
      : statements(std::move(statements)) {}

  void print() const {
    for (const auto& stmt : statements) {
      stmt->print();
    }
  }
};
