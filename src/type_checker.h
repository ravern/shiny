#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <iostream>
#include <unordered_map>

#include "ast_node.h"
#include "type.h"

class TypeChecker {
  // TODO: find a way to perform a lookup without copying from converting string_view to string
  std::unordered_map<std::string, Type> typeEnv;

public:
  Type visit(const ASTNode& node) {
    switch (node.type) {
      case ASTNode::INT_EXPR:
        return visitInt(static_cast<const IntNode&>(node));
      case ASTNode::FLOAT_EXPR:
        return visitFloat(static_cast<const FloatNode&>(node));
      case ASTNode::VARIABLE_EXPR:
        return visitVariable(static_cast<const VariableNode&>(node));
      case ASTNode::BINARY_EXPR:
        return visitBinary(static_cast<const BinaryNode&>(node));
      case ASTNode::ASSIGNMENT_STMT:
        return visitAssignment(static_cast<const AssignmentNode&>(node));
      case ASTNode::EXPR_STMT:
        return visitExprStmt(static_cast<const ExprStmtNode&>(node));
      case ASTNode::PROGRAM:
        return visitProgram(static_cast<const ProgramNode&>(node));
      default:
        throw std::runtime_error("Unsupported ASTNode type in visitNode");
    }
  }

  Type visitInt(const IntNode& node) const {
    return TYPE_INT;
  }

  Type visitFloat(const FloatNode& node) const {
    return TYPE_FLOAT;
  }

  Type visitVariable(const VariableNode& node) {
    auto it = typeEnv.find(std::string(node.name));
    if (it == typeEnv.end()) {
      throw std::runtime_error("Error: Undefined variable '" + std::string(node.name) + "'");
    }
    return it->second;
  }

  Type visitBinary(const BinaryNode& node) {
    Type leftType = visit(*node.left);
    Type rightType = visit(*node.right);

    // Allow implicit promotion of TYPE_INT to TYPE_FLOAT
    if (leftType == TYPE_FLOAT && rightType == TYPE_INT) {
      rightType = TYPE_FLOAT; // Promote the right-hand side
    } else if (leftType == TYPE_INT && rightType == TYPE_FLOAT) {
      leftType = TYPE_FLOAT;  // Promote the left-hand side
    }

    if (leftType != rightType) {
      throw std::runtime_error("Type checking binary op failed");
    }
    return leftType;
  }

  Type visitAssignment(const AssignmentNode& node) {
    Type rightType = visit(*node.expr);
    if (node.typeAnnotation) {
      Type leftType = mapTypeAnnotation(node.typeAnnotation.value());

      // Allow implicit conversion from INT to FLOAT
      if (leftType == TYPE_FLOAT && rightType == TYPE_INT) {
        rightType = TYPE_FLOAT; // Promote the right-hand side
      }

      if (leftType != rightType) {
        throw std::runtime_error("Type checking assignment failed");
      }
      typeEnv[std::string(node.varName)] = leftType;
    } else {
      typeEnv[std::string(node.varName)] = rightType;
    }
    return TYPE_NONE;
  }

  Type visitExprStmt(const ExprStmtNode& node) {
    visit(*node.expr);
    return TYPE_NONE;
  }

  Type visitProgram(const ProgramNode& node) {
    for (const auto& stmt : static_cast<const ProgramNode&>(node).statements) {
      visit(*stmt);
    }
    return TYPE_NONE;
  }

  void printTypeEnvironment() {
    std::cout << "Type Environment:\n";
    for (const auto& [var, type] : typeEnv) {
      std::cout << "  " << var << " : " << type << "\n";
    }
  }
};

#endif //TYPE_CHECKER_H
