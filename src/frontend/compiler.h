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

  std::vector<VariableName>& globals;
  StringInterner& stringInterner;
  Stmt& ast;

  FunctionObject function;
  std::optional<VariableName> name;

 public:
  Compiler(Compiler* enclosing_compiler, FunctionKind kind,
           std::vector<VariableName>& globals, StringInterner& stringInterner,
           Stmt& ast, std::optional<SymbolId> name = std::nullopt)
      : enclosingCompiler(enclosing_compiler),
        kind(kind),
        stringInterner(stringInterner),
        ast(ast),
        name(name),
        globals(globals),
        function(name) {}

  FunctionObject compile() {
    switch (kind) {
      case FunctionKind::TopLevel: {
        assert(ast.kind == StmtKind::Block);
        visit(ast);
        emit(Opcode::HALT);
        break;
      }
      case FunctionKind::Function:
      case FunctionKind::Method: {
        assert(ast.kind == StmtKind::Function);
        auto& functionStmt = static_cast<FunctionStmt&>(ast);
        if (functionStmt.params.size() > 255) {
          throw std::runtime_error("Too many function parameters");
        }
        // Why do we do this? I forgot
        // auto name = stringInterner.intern("__function__");
        // declare(name);
        // define(name);

        if (kind == FunctionKind::Method) {
          auto self = stringInterner.intern("self");
          auto local = Local(self, 0, false);
          locals.push_back(local);
        } else {
          auto local = Local(0, 0, false); // 0 is reserved for empty strings
          locals.push_back(local);
        }

        for (int i = 0; i < functionStmt.params.size(); ++i) {
          auto& param = functionStmt.params.at(i);
          declare(param.name);
          define(param.name);
        }
        visit(*functionStmt.body);
        emit(Opcode::NIL);
        emit(Opcode::RETURN);
        break;
      }
      default:
        throw std::runtime_error("Unknown FunctionKind");
    }

    std::string chunkName;
    if (name.has_value()) {
      chunkName = stringInterner.get(name.value());
    } else if (kind == FunctionKind::TopLevel) {
      chunkName = "<top level>";
    } else {
      chunkName = "<anonymous>";
    }

    std::cout << chunkToString(function.getChunk(), chunkName, stringInterner)
              << std::endl;

    return function;
  }

  // Expression visitors
  std::shared_ptr<Type> visitVoidExpr(IntegerExpr& expr) {
    emit(Opcode::NIL);
    return T::Void();
  }

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
    resolve(name);
    return expr.var.type.value();
  }

  std::shared_ptr<Type> visitSelfExpr(SelfExpr& expr) {
    SymbolId name = stringInterner.intern("self");
    resolve(name);
    assert(expr.type.has_value());
    return expr.type.value();
  }

  void resolve(SymbolId name) {
    int index = resolveLocal(name);
    if (index != -1) {
      emit(Opcode::LOAD, index);
    } else if ((index = resolveUpvalue(name)) != -1) {
      emit(Opcode::UPVALUE_LOAD, index);
    } else if ((index = resolveGlobal(name)) != -1) {
      emit(Opcode::GLOBAL_LOAD, index);
    } else {
      throw std::runtime_error(
          "Variable name not found");  // this should never happen; caught by
                                       // TypeInference
    }
  }

  std::shared_ptr<Type> visitApplyExpr(ApplyExpr& expr) {
    auto calleeType = visit(*expr.callee);
    assert(calleeType->kind == TypeKind::Function || calleeType->kind == TypeKind::Class);

    if (calleeType->kind == TypeKind::Function) {
      auto& functionType = static_cast<FunctionType&>(*calleeType);

      for (auto& arg : expr.arguments) {
        visit(*arg);
      }
      emit(Opcode::CALL, static_cast<uint32_t>(expr.arguments.size()));

      return functionType.ret;
    }

    if (calleeType->kind == TypeKind::Class) {
      std::shared_ptr<ClassType> classType = static_pointer_cast<ClassType>(calleeType);
      assert(expr.arguments.size() == 0);
      emit(Opcode::CALL, 0);

      return std::make_shared<InstanceType>(classType);
    }

    throw std::runtime_error("Target is not callable.");
  }

  std::shared_ptr<Type> visitBinaryExpr(BinaryExpr& expr) {
    auto lhsType = visit(*expr.left);
    visit(*expr.right);

    switch (expr.op) {
      case BinaryOperator::Add:
        emit(Opcode::ADD, lhsType);
        return lhsType;
      case BinaryOperator::Minus:
        emit(Opcode::SUB, lhsType);
        return lhsType;
      case BinaryOperator::Multiply:
        emit(Opcode::MUL, lhsType);
        return lhsType;
      case BinaryOperator::Divide:
        emit(Opcode::DIV, lhsType);
        return lhsType;
      case BinaryOperator::Modulo:
        emit(Opcode::MOD);
        return lhsType;
      case BinaryOperator::And:
        emit(Opcode::AND);
        return lhsType;
      case BinaryOperator::Or:
        emit(Opcode::OR);
        return lhsType;
      case BinaryOperator::Eq:
        emit(Opcode::EQ);
        return T::Bool();
      case BinaryOperator::Neq:
        emit(Opcode::NEQ);
        return T::Bool();
      case BinaryOperator::Lt:
        emit(Opcode::LT, lhsType);
        return T::Bool();
      case BinaryOperator::Lte:
        emit(Opcode::LTE, lhsType);
        return T::Bool();
      case BinaryOperator::Gt:
        emit(Opcode::GT, lhsType);
        return T::Bool();
      case BinaryOperator::Gte:
        emit(Opcode::GTE, lhsType);
        return T::Bool();
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

  std::shared_ptr<Type> visitAssignExpr(AssignExpr& expr) {
    throw std::runtime_error("Not implemented");
  }

  std::shared_ptr<Type> visitGetExpr(GetExpr& expr) {
    auto objType = visit(*expr.obj);
    assert(objType->kind == TypeKind::Instance);
    auto& instanceType = static_cast<InstanceType&>(*objType);
    auto& klass= instanceType.klass;
    auto memberIndex = klass->getMemberIndex(expr.name.name);
    assert(memberIndex != -1);
    auto memberType = klass->getMemberType(memberIndex).value();
    emit(Opcode::MEMBER_GET, memberIndex);
    return memberType;
  }

  std::shared_ptr<Type> visitSetExpr(SetExpr& expr) {
    throw std::runtime_error("Not implemented");
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

  void visitAssignStmt(AssignStmt& stmt) {
    visit(*stmt.expression);

    auto name = stmt.var.name;
    int index = resolveLocal(name);
    if (index != -1) {
      emit(Opcode::STORE, index);
    } else if ((index = resolveUpvalue(name) != -1)) {
      emit(Opcode::UPVALUE_STORE, index);
    } else if ((index = resolveGlobal(name) != -1)) {
      emit(Opcode::GLOBAL_STORE, index);
    } else {
      throw std::runtime_error(
          "Variable name not found");  // this should never happen; caught by
                                       // TypeInference
    }
  }

  void visitFunction(FunctionStmt& stmt, FunctionKind kind) {
    auto name = stmt.name.name;
    declare(name);
    defineWithoutEmitIfGlobal(name);  // allow recursion

    auto compiler = Compiler(this, kind, globals,
                             stringInterner, stmt, name);
    auto function = compiler.compile();

    uint32_t constantIndex =
        addConstant(ObjectPtr<FunctionObject>(std::move(function)));
    if (kind == FunctionKind::Function) {
      emit(Opcode::CLOSURE, constantIndex);
    } else {
      emit(Opcode::METHOD, constantIndex);
    }

    define(name, false);
  }

  void visitFunctionStmt(FunctionStmt& stmt) {
    visitFunction(stmt, FunctionKind::Function);
  }

  void visitClassStmt(ClassStmt& stmt) {
    auto name = stmt.name.name;
    declare(name);
    defineWithoutEmitIfGlobal(name);  // allow recursion

    uint32_t constantIndex = addConstant(ObjectPtr<ClassObject>(std::move(ClassObject(name))));
    emit(Opcode::CLASS, constantIndex);

    // emit METHODs (which keep the class on the stack)
    beginScope();

    auto initializerName = stringInterner.intern("init");
    auto initializerVar = Var(initializerName);

    // create a fake initializer function to compile and emit
    // TODO: just parse the damn thing and not do this
    std::vector<std::unique_ptr<Stmt>> declarations;
    declarations.reserve(stmt.declarations.size());
    // move stmt.declarations into declarations
    for (auto& decl : stmt.declarations) {
      declarations.push_back(std::move(decl));
    }

    auto blockStmt = std::make_unique<BlockStmt>(std::move(declarations));
    std::vector<Var> params;
    auto initializerAst = std::make_unique<FunctionStmt>(initializerVar, params, T::Void(), std::move(blockStmt));

    Compiler compiler(this, FunctionKind::Method, globals, stringInterner, *initializerAst);
    auto initializer = compiler.compile();

    uint32_t initializerIndex = addConstant(ObjectPtr<FunctionObject>(std::move(initializer)));
    emit(Opcode::METHOD, initializerIndex);

    // restore stmt.declarations
    stmt.declarations.clear();
    for (auto& stmtPtr : initializerAst->body->statements) {
      auto* declarePtr = static_cast<DeclareStmt*>(stmtPtr.release()); // safe since we know they were DeclareStmt
      stmt.declarations.push_back(std::unique_ptr<DeclareStmt>(declarePtr));
    }

    for (auto& method : stmt.methods) {
      visitFunction(*method, FunctionKind::Method);
    }

    endScope();

    // store and pop off the stack
    define(name, false);
  }

  void visitExprStmt(ExprStmt& stmt) {
    visit(*stmt.expression);
    emit(Opcode::POP);
  }

  void visitReturnStmt(ReturnStmt& stmt) {
    if (kind == FunctionKind::TopLevel) {
      throw std::runtime_error("Return invalid outside of a func");
    }
    visit(*stmt.expression);
    emit(Opcode::RETURN);
  }

  void visitIfStmt(IfStmt& stmt) {
    visit(*stmt.condition);
    emit(Opcode::TEST);

    size_t jumpToElseOffsetIndex = function.getChunk().instructions.size();
    emit(Opcode::JUMP, 0);  // will be patched later

    visit(*stmt.thenBranch);

    size_t jumpToEndOffsetIndex = -1;
    if (stmt.elseBranch.has_value()) {
      jumpToEndOffsetIndex = function.getChunk().instructions.size();
      emit(Opcode::JUMP, 0);  // will be patched later
    }

    size_t elseStart = function.getChunk().instructions.size();
    patchJump(jumpToElseOffsetIndex, elseStart);

    if (stmt.elseBranch.has_value()) {
      visit(*stmt.elseBranch.value());

      size_t end = function.getChunk().instructions.size();
      patchJump(jumpToEndOffsetIndex, end);
    }
  }

 private:
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

  int resolveGlobal(VariableName name) {
    if (kind != FunctionKind::TopLevel) {
      return enclosingCompiler->resolveGlobal(name);
    }

    for (int i = 0; i < globals.size(); i++) {
      auto& global = globals.at(i);
      if (global == name) {
        return i;
      }
    }

    return -1;
  }

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
    if (isTopLevel()) {
      return;
    }

    // check that local variable is unique in the current scope
    // note that all variables are local variables
    for (int i = locals.size() - 1; i >= 0; i--) {
      auto& local = locals.at(i);
      if (local.depth != -1 && local.depth < scopeDepth) {
        break;
      }

      // this feels like it should be checked in an earlier phase
      if (local.name == name) {
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

  void defineWithoutEmitIfGlobal(VariableName name) {
    if (isTopLevel()) {
      globals.push_back(name);
    }
  }

  void define(VariableName name, bool emitIfGlobal = true) {
    if (isTopLevel()) {
      if (emitIfGlobal) {
        globals.push_back(name);
      }
      emit(Opcode::GLOBAL_STORE, globals.size() - 1);
      return;
    }

    auto& local = locals.back();
    assert(local.name == name);
    local.depth = scopeDepth;
  }

  bool isTopLevel() {
    return scopeDepth == 0 && kind == FunctionKind::TopLevel;
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
    assertFits24BitOperand(operand);
    uint32_t instruction = static_cast<uint32_t>(opcode) | (operand << 8);
    function.getChunk().instructions.push_back(instruction);
  }

  void emit(Opcode opcode, const std::shared_ptr<Type>& type) {
    uint32_t opType = type->kind == TypeKind::Integer ? 1
                      : type->kind == TypeKind::Double
                          ? 2
                          : throw std::runtime_error("Unexpected TypeKind");
    emit(opcode, opType);
  }

  void patchJump(size_t jumpIndex, size_t targetIndex) {
    assertFits24BitOperand(targetIndex);
    uint32_t& instruction = function.getChunk().instructions[jumpIndex];
    uint32_t opcode = instruction & 0xFF;
    uint32_t offset = static_cast<uint32_t>(targetIndex);
    instruction = opcode | (offset << 8);
  }

  void assertFits24BitOperand(uint32_t operand) {
    constexpr uint32_t MAX_OPERAND = 0xFFFFFF;
    if (operand > MAX_OPERAND) {
      throw std::runtime_error("Operand exceeds 24-bit limit");
    }
  }
};
#endif  // COMPILER_H
