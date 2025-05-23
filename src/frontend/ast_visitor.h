#ifndef AST_VISITOR_H
#define AST_VISITOR_H
#include "expr.h"
#include "stmt.h"

template <typename ImplClass,
          typename ExprRetTy = void,
          typename StmtRetTy = void,
          typename... Args>
class ASTVisitor {
public:
  ExprRetTy visit(Expr& expr, Args... args) {
    switch (expr.kind) {
      case ExprKind::Void:
        return static_cast<ImplClass*>(this)->visitVoidExpr(static_cast<IntegerExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Integer:
        return static_cast<ImplClass*>(this)->visitIntegerExpr(static_cast<IntegerExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Double:
        return static_cast<ImplClass*>(this)->visitDoubleExpr(static_cast<DoubleExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Boolean:
        return static_cast<ImplClass*>(this)->visitBoolExpr(static_cast<BoolExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Variable:
        return static_cast<ImplClass*>(this)->visitVariableExpr(static_cast<VariableExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Self:
        return static_cast<ImplClass*>(this)->visitSelfExpr(static_cast<SelfExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Apply:
        return static_cast<ImplClass*>(this)->visitApplyExpr(static_cast<ApplyExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Binary:
        return static_cast<ImplClass*>(this)->visitBinaryExpr(static_cast<BinaryExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Unary:
        return static_cast<ImplClass*>(this)->visitUnaryExpr(static_cast<UnaryExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Assign:
        return static_cast<ImplClass*>(this)->visitAssignExpr(static_cast<AssignExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Get:
        return static_cast<ImplClass*>(this)->visitGetExpr(static_cast<GetExpr&>(expr), std::forward<Args>(args)...);
      case ExprKind::Set:
        return static_cast<ImplClass*>(this)->visitSetExpr(static_cast<SetExpr&>(expr), std::forward<Args>(args)...);
      default:
        throw std::runtime_error("Unknown ExprKind");
    }
  }

  StmtRetTy visit(Stmt& stmt, Args... args) {
    switch (stmt.kind) {
      case StmtKind::Block:
        return static_cast<ImplClass*>(this)->visitBlockStmt(static_cast<BlockStmt&>(stmt), std::forward<Args>(args)...);
      case StmtKind::Declare:
        return static_cast<ImplClass*>(this)->visitDeclareStmt(static_cast<DeclareStmt&>(stmt), std::forward<Args>(args)...);
      case StmtKind::Function:
        return static_cast<ImplClass*>(this)->visitFunctionStmt(static_cast<FunctionStmt&>(stmt), std::forward<Args>(args)...);
      case StmtKind::Class:
        return static_cast<ImplClass*>(this)->visitClassStmt(static_cast<ClassStmt&>(stmt), std::forward<Args>(args)...);
      case StmtKind::Expr:
        return static_cast<ImplClass*>(this)->visitExprStmt(static_cast<ExprStmt&>(stmt), std::forward<Args>(args)...);
      case StmtKind::Return:
        return static_cast<ImplClass*>(this)->visitReturnStmt(static_cast<ReturnStmt&>(stmt), std::forward<Args>(args)...);
      case StmtKind::If:
        return static_cast<ImplClass*>(this)->visitIfStmt(static_cast<IfStmt&>(stmt), std::forward<Args>(args)...);
      default:
        throw std::runtime_error("Unknown StmtKind");
    }
  }
};

#endif //AST_VISITOR_H
