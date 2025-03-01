#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast_node.h"

class TypeChecker {
public:
  void visitNode(const ASTNode& node) const {
    switch (node.type) {
      case ASTNode::NUMBER_EXPR:
        visitNumberNode(static_cast<const NumberNode&>(node));
      break;
      case ASTNode::VARIABLE_EXPR:
        visitVariableNode(static_cast<const VariableNode&>(node));
      break;
      case ASTNode::BINARY_EXPR:
        visitBinaryNode(static_cast<const BinaryNode&>(node));
      break;
      case ASTNode::ASSIGNMENT_STMT:
        visitAssignmentNode(static_cast<const AssignmentNode&>(node));
      break;
      case ASTNode::EXPR_STMT:
        visitExprStmtNode(static_cast<const ExprStmtNode&>(node));
      break;
      case ASTNode::PROGRAM:
        visitProgramNode(static_cast<const ProgramNode&>(node));
      break;
      default:
        throw std::runtime_error("Unsupported ASTNode type in visitNode");
    }
  }

  void visitNumberNode(const NumberNode& node) const {
  }

  void visitVariableNode(const VariableNode& node) const {
  }

  void visitBinaryNode(const BinaryNode& node) const {
    visitNode(*node.left);
    visitNode(*node.right);
  }

  void visitAssignmentNode(const AssignmentNode& node) const {
    visitNode(*node.expr);
  }

  void visitExprStmtNode(const ExprStmtNode& node) const {
    visitNode(*node.expr);
  }

  void visitProgramNode(const ProgramNode& node) const {
    for (const auto& stmt : static_cast<const ProgramNode&>(node).statements) {
      visitNode(*stmt);
    }
  }
};



#endif //TYPE_CHECKER_H
