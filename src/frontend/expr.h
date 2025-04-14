#ifndef EXPR_H
#define EXPR_H

#include <utility>

#include "type.h"
#include "var.h"

enum class ExprKind {
  Void,
  Integer,
  Double,
  Boolean,
  Variable,
  Self,
  Apply,
  Binary,
  Unary,
  Assign,
  Get,
  Set,
};

class Expr {
public:
  ExprKind kind;

  explicit Expr(ExprKind kind) : kind(kind) {}
  virtual ~Expr() = default;

  virtual bool operator==(const Expr& other) const {
    return kind == other.kind;
  }

  bool operator!=(const Expr& other) const = default;
};

class VoidExpr : public Expr {
public:
  VoidExpr() : Expr(ExprKind::Void) {}

  bool operator==(const Expr& other) const override {
    return kind == other.kind;
  }
};

class IntegerExpr : public Expr {
public:
  std::string_view literal;

  explicit IntegerExpr(std::string_view literal)
    : Expr(ExprKind::Integer), literal(literal) {}

  int64_t getValue() const {
    return std::stoll(std::string(literal));
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherInt = static_cast<const IntegerExpr&>(other);
    return literal == otherInt.literal;
  }
};

class DoubleExpr : public Expr {
public:
  std::string_view literal;

  explicit DoubleExpr(std::string_view literal)
    : Expr(ExprKind::Double), literal(literal) {}

  double getValue() const {
    return std::stod(std::string(literal));
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherDouble = static_cast<const DoubleExpr&>(other);
    return literal == otherDouble.literal;
  }
};

class BoolExpr : public Expr {
public:
  bool literal;

  explicit BoolExpr(bool literal)
    : Expr(ExprKind::Boolean), literal(literal) {}

  bool getValue() const {
    return literal;
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherBool = static_cast<const BoolExpr&>(other);
    return literal == otherBool.literal;
  }
};

class VariableExpr : public Expr {
public:
  Var var;

  explicit VariableExpr(Var var)
    : Expr(ExprKind::Variable),
      var(std::move(var)) {
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherVar = static_cast<const VariableExpr&>(other);
    return var == otherVar.var;
  }
};

class SelfExpr : public Expr {
public:
  std::optional<std::shared_ptr<Type>> type;

  explicit SelfExpr()
    : Expr(ExprKind::Self) {}

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    return true;
  }
};

class ApplyExpr : public Expr {
public:
  std::unique_ptr<Expr> callee;
  std::vector<std::unique_ptr<Expr>> arguments;

  ApplyExpr(std::unique_ptr<Expr> function, std::vector<std::unique_ptr<Expr>> arguments)
    : Expr(ExprKind::Apply),
      callee(std::move(function)),
      arguments(std::move(arguments)) {
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherApply = static_cast<const ApplyExpr&>(other);

    if (*callee != *otherApply.callee) return false;

    if (arguments.size() != otherApply.arguments.size()) {
      return false;
    }

    for (size_t i = 0; i < arguments.size(); i++) {
      if (*arguments[i] != *otherApply.arguments[i]) {
        return false;
      }
    }

    return true;
  }
};

enum class BinaryOperator {
  Add,
  Minus,
  Multiply,
  Divide,
  Modulo,
  And,
  Or,
  Eq,
  Neq,
  Lt,
  Lte,
  Gt,
  Gte
};

inline std::string toString(BinaryOperator op) {
  switch (op) {
    case BinaryOperator::Add: return "+";
    case BinaryOperator::Minus: return "-";
    case BinaryOperator::Multiply: return "*";
    case BinaryOperator::Divide: return "/";
    case BinaryOperator::Modulo: return "%";
    case BinaryOperator::And: return "&&";
    case BinaryOperator::Or: return "||";
    case BinaryOperator::Eq: return "==";
    case BinaryOperator::Neq: return "!=";
    case BinaryOperator::Lt: return "<";
    case BinaryOperator::Lte: return "<=";
    case BinaryOperator::Gt: return ">";
    case BinaryOperator::Gte: return ">=";
    default: return "Unknown BinaryOperator";
  }
}

class BinaryExpr : public Expr {
public:
  std::unique_ptr<Expr> left;
  BinaryOperator op;
  std::unique_ptr<Expr> right;

  BinaryExpr(std::unique_ptr<Expr> left, BinaryOperator op, std::unique_ptr<Expr> right)
    : Expr(ExprKind::Binary),
      left(std::move(left)),
      op(op),
      right(std::move(right)) {
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherBinary = static_cast<const BinaryExpr&>(other);

    // Compare left and right operands
    auto leftEqual = *left == *otherBinary.left;
    auto rightEqual = *right == *otherBinary.right;
    return leftEqual && rightEqual;
  }
};

enum class UnaryOperator {
  Not,
  Negate,
};

inline std::string toString(UnaryOperator op) {
  switch (op) {
    case UnaryOperator::Not: return "!";
    case UnaryOperator::Negate: return "-";
    default: return "Unknown UnaryOperator";
  }
}

class UnaryExpr : public Expr {
public:
  UnaryOperator op;
  std::unique_ptr<Expr> operand;

  UnaryExpr(UnaryOperator op, std::unique_ptr<Expr> operand)
    : Expr(ExprKind::Unary),
      op(op),
      operand(std::move(operand)) {
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherUnary = static_cast<const UnaryExpr&>(other);
    return op == otherUnary.op && *operand == *otherUnary.operand;
  }
};

class AssignExpr : public Expr {
public:
  Var var;
  std::unique_ptr<Expr> expression;

  AssignExpr(Var var, std::unique_ptr<Expr> expression)
    : Expr(ExprKind::Assign),
      var(std::move(var)),
      expression(std::move(expression)) {}

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }

    const auto& otherAssign = static_cast<const AssignExpr&>(other);
    return var == otherAssign.var &&
      *expression == *otherAssign.expression;
  }
};

class GetExpr : public Expr {
public:
  std::unique_ptr<Expr> obj;
  Var name;

  explicit GetExpr(std::unique_ptr<Expr> obj, Var var)
    : Expr(ExprKind::Get),
      obj(std::move(obj)), name(std::move(var)) {
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherGet = static_cast<const GetExpr&>(other);
    return *obj == *otherGet.obj && name == otherGet.name;
  }
};

class SetExpr : public Expr {
public:
  std::unique_ptr<Expr> obj;
  Var var;
  std::unique_ptr<Expr> value;

  explicit SetExpr(std::unique_ptr<Expr> obj, Var var, std::unique_ptr<Expr> value)
    : Expr(ExprKind::Set),
      obj(std::move(obj)), var(std::move(var)), value(std::move(value)) {
  }

  bool operator==(const Expr& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherSet = static_cast<const SetExpr&>(other);
    return *obj == *otherSet.obj && var == otherSet.var && *value == *otherSet.value;
  }
};

#endif //EXPR_H
