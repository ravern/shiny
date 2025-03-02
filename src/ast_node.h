#ifndef AST_NODE_H
#define AST_NODE_H
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

class ASTNode {
public:
  enum Type {
    INT_EXPR,
    FLOAT_EXPR,
    VARIABLE_EXPR,
    BINARY_EXPR,
    ASSIGNMENT_STMT,
    EXPR_STMT,
    PROGRAM
  };

  Type type;

  explicit ASTNode(Type type) : type(type) {}
};

class IntNode : public ASTNode {
public:
  std::string_view literal;

  explicit IntNode(std::string_view literal) : ASTNode(INT_EXPR), literal(literal) {}

  int getValue() const {
    return std::stoi(std::string(literal));
  }
};

class FloatNode : public ASTNode {
public:
  std::string_view literal;

  explicit FloatNode(std::string_view literal) : ASTNode(FLOAT_EXPR), literal(literal) {}

  float getValue() const {
    return std::stof(std::string(literal));  // Convert string_view to string before parsing
  }
};

class VariableNode : public ASTNode {
public:
  std::string_view name;

  explicit VariableNode(std::string_view name) : ASTNode(VARIABLE_EXPR), name(name) {}
};

class BinaryNode : public ASTNode {
public:
  char op;
  std::unique_ptr<ASTNode> left;
  std::unique_ptr<ASTNode> right;

  BinaryNode(char op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
  : ASTNode(BINARY_EXPR), op(op), left(std::move(left)), right(std::move(right)) {}
};

class AssignmentNode : public ASTNode {
public:
  std::string_view varName;
  std::optional<std::string_view> typeAnnotation;
  std::unique_ptr<ASTNode> expr;

  AssignmentNode(const std::string_view var_name,
                 const std::optional<std::string_view> type_annotation,
                 std::unique_ptr<ASTNode> expr)
      : ASTNode(ASSIGNMENT_STMT), varName(var_name), typeAnnotation(type_annotation), expr(std::move(expr)) {}
};

class ExprStmtNode : public ASTNode {
public:
  std::unique_ptr<ASTNode> expr;

  explicit ExprStmtNode(std::unique_ptr<ASTNode> expr)
  : ASTNode(ASTNode::EXPR_STMT), expr(std::move(expr)) {}
};

class ProgramNode : public ASTNode {
public:
  std::vector<std::unique_ptr<ASTNode>> statements;

  explicit ProgramNode(std::vector<std::unique_ptr<ASTNode>> statements)
      : ASTNode(PROGRAM), statements(std::move(statements)) {}
};

#endif //AST_NODE_H
