#ifndef STMT_H
#define STMT_H

#include "expr.h"
#include "var.h"

enum class StmtKind {
  Block,
  Declare,
  Function,
  Class,
  Expr,
  Return,
  If
};

class Stmt {
public:
  StmtKind kind;

  explicit Stmt(StmtKind kind) : kind(kind) {}
  virtual ~Stmt() = default;

  virtual bool operator==(const Stmt& other) const {
    return kind == other.kind;
  }

  bool operator!=(const Stmt& other) const = default;
};

class BlockStmt : public Stmt {
public:
  std::vector<std::unique_ptr<Stmt>> statements;

  explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
    : Stmt(StmtKind::Block),
      statements(std::move(statements)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) {
      return false;
    }

    const auto& otherBlock = static_cast<const BlockStmt&>(other);

    if (statements.size() != otherBlock.statements.size()) {
      return false;
    }

    for (size_t i = 0; i < statements.size(); i++) {
      if (*statements[i] != *otherBlock.statements[i]) {
        return false;
      }
    }

    return true;
  }
};

class DeclareStmt : public Stmt {
public:
  Var var;
  std::unique_ptr<Expr> expression;

  DeclareStmt(Var  var, std::unique_ptr<Expr> expression)
    : Stmt(StmtKind::Declare),
      var(std::move(var)),
      expression(std::move(expression)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) {
      return false;
    }

    const auto& otherDecl = static_cast<const DeclareStmt&>(other);
    return var == otherDecl.var &&
      *expression == *otherDecl.expression;
  }
};

class FunctionStmt : public Stmt {
public:
  Var name;
  std::vector<Var> params;
  std::shared_ptr<Type> returnType;
  std::unique_ptr<BlockStmt> body;

  FunctionStmt(Var name, std::vector<Var> params, std::shared_ptr<Type> returnType, std::unique_ptr<BlockStmt> body)
    : Stmt(StmtKind::Function),
      name(std::move(name)),
      params(std::move(params)),
      returnType(std::move(returnType)),
      body(std::move(body)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) {
      return false;
    }
    const auto& otherFunc = static_cast<const FunctionStmt&>(other);

    if (name != otherFunc.name) return false;
    if (params != otherFunc.params) return false;
    if (*returnType != *otherFunc.returnType) return false;
    if (*body != *otherFunc.body) return false;
    return true;
  }
};

class ClassStmt : public Stmt {
public:
  Var name;
  std::vector<std::unique_ptr<DeclareStmt>> declarations;
  std::vector<std::unique_ptr<FunctionStmt>> methods;
  std::optional<std::shared_ptr<ClassType>> type;

  ClassStmt(Var name,
            std::vector<std::unique_ptr<DeclareStmt>> declarations,
            std::vector<std::unique_ptr<FunctionStmt>> methods)
    : Stmt(StmtKind::Class),
      name(std::move(name)),
      declarations(std::move(declarations)),
      methods(std::move(methods)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) return false;
    const auto& otherClass = static_cast<const ClassStmt&>(other);

    if (name != otherClass.name) return false;

    if (declarations.size() != otherClass.declarations.size() ||
        methods.size() != otherClass.methods.size()) {
      return false;
    }

    for (size_t i = 0; i < declarations.size(); ++i) {
      if (*declarations[i] != *otherClass.declarations[i]) return false;
    }

    for (size_t i = 0; i < methods.size(); ++i) {
      if (*methods[i] != *otherClass.methods[i]) return false;
    }

    return true;
  }
};

class ExprStmt : public Stmt {
public:
  std::unique_ptr<Expr> expression;

  explicit ExprStmt(std::unique_ptr<Expr> expression)
    : Stmt(StmtKind::Expr),
      expression(std::move(expression)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) {
      return false;
    }

    const auto& otherStmt = static_cast<const ExprStmt&>(other);
    return *expression == *otherStmt.expression;
  }
};

class ReturnStmt : public Stmt {
public:
  std::unique_ptr<Expr> expression;

  explicit ReturnStmt(std::unique_ptr<Expr> expression)
    : Stmt(StmtKind::Return), expression(std::move(expression)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) {
      return false;
    }

    const auto& otherReturn = static_cast<const ReturnStmt&>(other);
    return *expression == *otherReturn.expression;
  }
};

class IfStmt : public Stmt {
public:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> thenBranch;
  std::optional<std::unique_ptr<Stmt>> elseBranch;

  IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch)
    : Stmt(StmtKind::If),
      condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::nullopt) {}

  IfStmt(std::unique_ptr<Expr> condition,
         std::unique_ptr<Stmt> thenBranch,
         std::optional<std::unique_ptr<Stmt>> elseBranch)
    : Stmt(StmtKind::If),
      condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

  bool operator==(const Stmt& other) const override {
    if (kind != other.kind) {
      return false;
    }

    const auto& otherIf = static_cast<const IfStmt&>(other);
    if (*condition != *otherIf.condition || *thenBranch != *otherIf.thenBranch) {
      return false;
    }

    if (elseBranch.has_value() != otherIf.elseBranch.has_value()) {
      return false;
    }

    if (elseBranch.has_value()) {
      return **elseBranch == **otherIf.elseBranch;
    }

    return true;
  }
};

#endif //STMT_H
