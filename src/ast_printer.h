#ifndef AST_PRINTER_H
#define AST_PRINTER_H
#include <iostream>
#include "ast_node.h"

class ASTPrinter {
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
    std::cout << node.literal;
  }

  void visitVariableNode(const VariableNode& node) const {
    std::cout << node.name;
  }

  void visitBinaryNode(const BinaryNode& node) const {
    std::cout << "(";
    visitNode(*node.left);
    std::cout << " " << node.op << " ";
    visitNode(*node.right);
    std::cout << ")";
  }

  void visitAssignmentNode(const AssignmentNode& node) const {
    std::cout << "var " << node.varName;
    if (node.typeAnnotation) {
      std::cout << ": " << *node.typeAnnotation;
    }
    std::cout << " = ";
    visitNode(*node.expr);
    std::cout << ";\n";
  }

  void visitExprStmtNode(const ExprStmtNode& node) const {
    visitNode(*node.expr);
    std::cout << ";\n";
  }

  void visitProgramNode(const ProgramNode& node) const {
    for (const auto& stmt : static_cast<const ProgramNode&>(node).statements) {
      visitNode(*stmt);
    }
  }
};

#endif //AST_PRINTER_H
