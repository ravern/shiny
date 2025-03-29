#ifndef COMPILER_H
#define COMPILER_H
#include <assert.h>

#include "bytecode.h"
#include "runtime/object.h"
#include "frontend/ast_visitor.h"
#include "frontend/stmt.h"

struct Local {
  VariableName name;
  int depth; // -1 is undefined
  bool isCaptured;

  Local(VariableName name, int depth, bool is_captured)
      : name(name), depth(depth), isCaptured(is_captured) {}
};

class Compiler : public ASTVisitor<Compiler, void, void> {
public:
  enum class FunctionKind {
    Script,
    Function,
    Method,
    Initializer
  };

private:
  Compiler* enclosingCompiler;
  FunctionKind kind;
  std::vector<Local> locals;
  int scopeDepth = 0;

  StringInterner& stringInterner;
  Stmt& ast;

  FunctionObject function;

 public:
  Compiler(Compiler* enclosing_compiler,
           FunctionKind kind,
           StringInterner& stringInterner,
           Stmt& ast)
      : enclosingCompiler(enclosing_compiler),
        kind(kind),
        stringInterner(stringInterner),
        ast(ast),
        function({ 0 }) {}

  FunctionObject compile() {
    if (kind == FunctionKind::Script) {
      assert(ast.kind == StmtKind::Block);
    }
    visit(ast);
    emit(Opcode::RETURN);
    return function;
  }

  // Expression visitors
  void visitIntegerExpr(IntegerExpr& expr) {
    int64_t value = expr.getValue();
    uint32_t constantIndex = addConstant(value);
    emit(Opcode::CONST, constantIndex);
  }

  void visitDoubleExpr(DoubleExpr& expr) {
    double value = expr.getValue();
    uint32_t constantIndex = addConstant(value);
    emit(Opcode::CONST, constantIndex);
  }

  void visitBoolExpr(BoolExpr& expr) {
    emit(expr.getValue() ? Opcode::TRUE : Opcode::FALSE);
  }

  void visitVariableExpr(VariableExpr& expr) {
    int index = resolve(expr.var.name);
    if (index == -1) {
      throw std::runtime_error("Variable name not found"); // this should never happen; caught by TypeInference
    }
    emit(Opcode::STORE, index);
  }

  void visitApplyExpr(ApplyExpr& expr) {
    for (auto& arg : expr.arguments) {
      visit(*arg);
    }
    visit(*expr.function);
    emit(Opcode::CALL, static_cast<uint32_t>(expr.arguments.size()));
  }

  void visitBinaryExpr(BinaryExpr& expr) {
    visit(*expr.left);
    visit(*expr.right);

    switch (expr.op) {
      case BinaryOperator::Add:
        emit(Opcode::ADD);
        break;
      case BinaryOperator::Minus:
        emit(Opcode::SUB);
        break;
      case BinaryOperator::And:
        emit(Opcode::AND);
        break;
      case BinaryOperator::Or:
        emit(Opcode::OR);
        break;
    }
  }

  void visitUnaryExpr(UnaryExpr& expr) {
    visit(*expr.operand);

    switch (expr.op) {
      case UnaryOperator::Negate:
        emit(Opcode::NEG);
        break;
      case UnaryOperator::Not:
        emit(Opcode::NOT);
        break;
    }
  }

  // Statement visitors
  void visitBlockStmt(BlockStmt& stmt) {
    for (auto& statement : stmt.statements) {
      visit(*statement);
    }
  }

  void visitDeclareStmt(DeclareStmt& stmt) {
    declare(stmt.var.name);
    visit(*stmt.expression);
    define(stmt.var.name);
  }

  int resolve(VariableName name) {
    for (int i = locals.size() - 1; i >= 0; i--) {
      auto& local = locals.at(i);
      if (local.name == name) {
        if (local.depth == -1) {
          throw std::runtime_error("Circular reference"); // this should never happen; caught by TypeInference
        }
        return i;
      }
    }
    return -1;
  }

  void declare(VariableName name) {
    // check that local variable is unique in the current scope
    // note that all variables are local variables
    for (int i = locals.size() - 1; i >= 0; i--) {
      auto& local = locals.at(i);
      if (local.depth != -1 && local.depth < scopeDepth) {
        break;
      }

      if (local.name == name) { // this feels like it should be checked in an earlier phase
        throw std::runtime_error("Invalid redeclaration of '" + stringInterner.get(name) + "'");
      }
    }

    if (locals.size() == 256) {
      throw std::runtime_error("Too many local variables");
    }
    auto local = Local(name, -1, false);
    locals.push_back(local);
  }

  void define(VariableName name) {
    auto& local = locals.back();
    local.depth = scopeDepth;
  }

  void visitAssignStmt(AssignStmt& stmt) {}

  void visitFunctionStmt(FunctionStmt& stmt) {}

  void visitReturnStmt(ReturnStmt& stmt) {
    visit(*stmt.expression);
    emit(Opcode::RETURN);
  }

  void visitIfStmt(IfStmt& stmt) {}

 private:
  void emitConstant(Value constant) {
    uint32_t index = addConstant(constant);
    emit(Opcode::CONST, index);
  }

  uint32_t addConstant(Value constant) {
    function.chunk.constants.push_back(constant);
    auto index = static_cast<uint32_t>(function.chunk.constants.size() - 1);

    // Check if index fits in 3 bytes (24 bits)
    constexpr uint32_t MAX_THREE_BYTES = 0xFFFFFF;  // 16,777,215
    if (index > MAX_THREE_BYTES) {
      throw std::runtime_error(
          "Too many constants in chunk. Maximum index must fit in 3 bytes.");
    }

    return index;
  }

  void emit(Opcode opcode, uint32_t operand = 0) {
    uint32_t instruction = static_cast<uint32_t>(opcode) | (operand << 8);
    function.chunk.instructions.push_back(instruction);
  }
};
#endif //COMPILER_H
