#ifndef COMPILER_H
#define COMPILER_H
#include <assert.h>

#include "../bytecode.h"
#include "../debug.h"
#include "../frontend/ast_visitor.h"
#include "../frontend/factory.h"
#include "../frontend/stmt.h"
#include "../runtime/object.h"
#include "string_interner.h"

struct Local {
  VariableName name;
  int depth;  // -1 is undefined
  bool isCaptured;

  Local(VariableName name, int depth, bool is_captured)
      : name(name), depth(depth), isCaptured(is_captured) {}
};

class Compiler : public ASTVisitor<Compiler, std::shared_ptr<Type>, void> {
 public:
  enum class FunctionKind { TopLevel, Function, Method, Initializer };

 private:
  Compiler* enclosingCompiler;
  FunctionKind kind;
  std::vector<Local> locals;
  int scopeDepth = 0;  // starts from zero for every Compiler/function.

  StringInterner& stringInterner;
  Stmt& ast;

  FunctionObject function;
  std::optional<SymbolId> name;

 public:
  Compiler(Compiler* enclosing_compiler, FunctionKind kind,
           StringInterner& stringInterner, Stmt& ast,
           std::optional<SymbolId> name = std::nullopt)
      : enclosingCompiler(enclosing_compiler),
        kind(kind),
        stringInterner(stringInterner),
        ast(ast),
        function(0),
        name(name) {}

  FunctionObject compile() {
    switch (kind) {
      case FunctionKind::TopLevel: {
        assert(ast.kind == StmtKind::Block);
        visit(ast);
        break;
      }
      case FunctionKind::Function: {
        assert(ast.kind == StmtKind::Function);
        auto functionStmt = static_cast<FunctionStmt&>(ast);
        if (functionStmt.params.size() > 255) {
          throw std::runtime_error("Too many function parameters");
        }
        declare(stringInterner.intern("__function__"));
        define();
        for (int i = 0; i < functionStmt.params.size(); ++i) {
          auto& param = functionStmt.params.at(i);
          declare(param.name);
          define();
        }
        visit(*functionStmt.body);
        break;
      }
      default:
        throw std::runtime_error("Unknown FunctionKind");
    }

    emit(Opcode::NIL);
    emit(Opcode::RETURN);

    disassembleChunk(function.getChunk(),
                     name ? stringInterner.get(name.value()) : "<function>");

    return function;
  }

  // Expression visitors
  std::shared_ptr<Type> visitIntegerExpr(IntegerExpr& expr) {
    int64_t value = expr.getValue();
    uint32_t constantIndex = addConstant(value);
    emit(Opcode::CONST, constantIndex);

    return T::Int();
  }

  std::shared_ptr<Type> visitDoubleExpr(DoubleExpr& expr) {
    double value = expr.getValue();
    uint32_t constantIndex = addConstant(value);
    emit(Opcode::CONST, constantIndex);

    return T::Double();
  }

  std::shared_ptr<Type> visitBoolExpr(BoolExpr& expr) {
    emit(expr.getValue() ? Opcode::TRUE : Opcode::FALSE);

    return T::Bool();
  }

  std::shared_ptr<Type> visitVariableExpr(VariableExpr& expr) {
    auto name = expr.var.name;
    int index = resolveLocal(name);
    if (index != -1) {
      emit(Opcode::LOAD, index);
    } else if ((index = resolveUpvalue(name) != -1)) {
      emit(Opcode::UPVALUE_LOAD, index);
    } else {
      throw std::runtime_error(
          "Variable name not found");  // this should never happen; caught by
                                       // TypeInference
    }

    return expr.var.type.value();
  }

  std::shared_ptr<Type> visitApplyExpr(ApplyExpr& expr) {
    visit(*expr.function);
    for (auto& arg : expr.arguments) {
      visit(*arg);
    }
    emit(Opcode::CALL, static_cast<uint32_t>(expr.arguments.size()));

    auto var = static_cast<VariableExpr*>(expr.function.get());
    return var->var.type.value();
  }

  std::shared_ptr<Type> visitBinaryExpr(BinaryExpr& expr) {
    auto lhsType = visit(*expr.left);
    visit(*expr.right);

    switch (expr.op) {
      case BinaryOperator::Add: {
        uint32_t opType = lhsType->kind == TypeKind::Integer ? 1
                          : lhsType->kind == TypeKind::Double
                              ? 2
                              : throw std::runtime_error("Unexpected TypeKind");
        emit(Opcode::ADD, opType);
        return lhsType;
      }
      case BinaryOperator::Minus: {
        uint32_t opType = lhsType->kind == TypeKind::Integer ? 1
                          : lhsType->kind == TypeKind::Double
                              ? 2
                              : throw std::runtime_error("Unexpected TypeKind");
        emit(Opcode::SUB, opType);
        return lhsType;
      }
      case BinaryOperator::And:
        emit(Opcode::AND);
        return lhsType;
      case BinaryOperator::Or:
        emit(Opcode::OR);
        return lhsType;
      default:
        throw std::runtime_error("Unknown BinaryOperator");
    }
  }

  std::shared_ptr<Type> visitUnaryExpr(UnaryExpr& expr) {
    auto type = visit(*expr.operand);

    switch (expr.op) {
      case UnaryOperator::Negate:
        emit(Opcode::NEG);
        return type;
      case UnaryOperator::Not:
        emit(Opcode::NOT);
        return type;
      default:
        throw std::runtime_error("Unknown UnaryOperator");
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
    define();
  }

  void visitAssignStmt(AssignStmt& stmt) {
    visit(*stmt.expression);

    auto name = stmt.var.name;
    int index = resolveLocal(name);
    if (index != -1) {
      emit(Opcode::STORE, index);
    } else if ((index = resolveUpvalue(name) != -1)) {
      emit(Opcode::UPVALUE_STORE, index);
    } else {
      throw std::runtime_error(
          "Variable name not found");  // this should never happen; caught by
                                       // TypeInference
    }
  }

  void visitFunctionStmt(FunctionStmt& stmt) {
    auto name = stmt.name.name;
    declare(name);
    define();  // function body can reference itself

    auto compiler =
        Compiler(this, FunctionKind::Function, stringInterner, stmt, name);
    auto function = compiler.compile();

    uint32_t constantIndex = addConstant(ObjectRef(std::move(function)));
    emit(Opcode::CLOSURE, constantIndex);
  }

  void visitReturnStmt(ReturnStmt& stmt) {
    if (kind == FunctionKind::TopLevel) {
      throw std::runtime_error("Return invalid outside of a func");
    }
    visit(*stmt.expression);
    emit(Opcode::RETURN);
  }

  void visitIfStmt(IfStmt& stmt) {}

  int resolveLocal(VariableName name) {
    for (int i = locals.size() - 1; i >= 0; i--) {
      auto& local = locals.at(i);
      if (local.name == name) {
        if (local.depth == -1) {
          throw std::runtime_error(
              "Circular reference");  // this should never happen; caught by
                                      // TypeInference
        }
        return i;
      }
    }
    return -1;
  }

 private:
  void beginScope() { scopeDepth++; }

  void endScope() {
    scopeDepth--;
    while (locals.size() > 0 && locals.back().depth > scopeDepth) {
      auto& local = locals.back();
      if (local.isCaptured) {
        emit(Opcode::UPVALUE_CLOSE);
      } else {
        emit(Opcode::POP);
      }
      locals.pop_back();
    }
  }

  int resolveUpvalue(VariableName name) {
    if (enclosingCompiler == nullptr) {
      return -1;
    }
    int local = enclosingCompiler->resolveLocal(name);
    if (local != -1) {
      enclosingCompiler->locals[local].isCaptured = true;
      return function.addUpvalue(Upvalue{local, true});
    }

    int upvalue = enclosingCompiler->resolveUpvalue(name);
    if (upvalue != -1) {
      return function.addUpvalue(Upvalue{upvalue, false});
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

      if (local.name ==
          name) {  // this feels like it should be checked in an earlier phase
        throw std::runtime_error("Invalid redeclaration of '" +
                                 stringInterner.get(name) + "'");
      }
    }

    if (locals.size() == 256) {
      throw std::runtime_error("Too many local variables");
    }
    auto local = Local(name, -1, false);
    locals.push_back(local);
  }

  void define() {
    auto& local = locals.back();
    local.depth = scopeDepth;
  }

  void emitConstant(Value constant) {
    uint32_t index = addConstant(constant);
    emit(Opcode::CONST, index);
  }

  uint32_t addConstant(Value constant) {
    function.getChunk().constants.push_back(constant);
    auto index =
        static_cast<uint32_t>(function.getChunk().constants.size() - 1);

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
    function.getChunk().instructions.push_back(instruction);
  }
};
#endif  // COMPILER_H
