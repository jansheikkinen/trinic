/* statementAST.c */

#include <stdio.h>

#include "statementAST.h"

static struct StmtAST newExpression(struct ExprAST* expr) {
  struct StmtAST stmt;

  struct ExprStmt expression;
  expression.expression = expr;

  stmt.type = STMT_EXPRESSION;
  stmt.as.expression = expression;

  return stmt;
}

static struct StmtAST newBuiltin(enum BuiltinType type, struct ExprAST* parameter) {
  struct StmtAST stmt;

  struct BuiltinStmt builtin;
  builtin.type = type;
  builtin.parameter = parameter;

  stmt.type = STMT_BUILTIN;
  stmt.as.builtin = builtin;

  return stmt;
}

// I promise VarDecl and Assign are(will be) different
static struct StmtAST newVarDecl(const char* identifier, struct ExprAST* expr) {
  struct StmtAST stmt;

  struct VarDeclStmt vardecl;
  vardecl.identifier = identifier;
  vardecl.value = expr;

  stmt.type = STMT_VARDECL;
  stmt.as.vardecl = vardecl;

  return stmt;
}

static struct StmtAST newAssign(const char* identifier, struct ExprAST* expr) {
  struct StmtAST stmt;

  struct VarAssignStmt assign;
  assign.identifier = identifier;
  assign.value = expr;

  stmt.type = STMT_VARASSIGN;
  stmt.as.assignment = assign;

  return stmt;
}

struct StmtAST* allocNewExpression(struct ExprAST* expr) {
  struct StmtAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newExpression(expr);

  return ptr;
}

struct StmtAST* allocNewBuiltin(enum BuiltinType type, struct ExprAST* expr) {
  struct StmtAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newBuiltin(type, expr);

  return ptr;
}

struct StmtAST* allocNewVarDecl(const char* identifier,
    struct ExprAST* expr) {
  struct StmtAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newVarDecl(identifier, expr);

  return ptr;
}

struct StmtAST* allocNewAssign(const char* identifier,
    struct ExprAST* expr) {
  struct StmtAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newAssign(identifier, expr);

  return ptr;
}

void printStmtAST(const struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED:
      printf("(STMT_UNDEFINED)\n"); break;
    case STMT_EXPRESSION:
      printf("(EXPR ");
      printExprAST(stmt->as.expression.expression);
      printf(")\n"); break;
    case STMT_BUILTIN:
      printf("(BUILTIN_%d ", stmt->as.builtin.type);
      printExprAST(stmt->as.builtin.parameter);
      printf(")\n"); break;
    case STMT_VARDECL:
      printf("(VARDECL (%s) ", stmt->as.vardecl.identifier);
      printExprAST(stmt->as.vardecl.value);
      printf(")\n"); break;
    case STMT_VARASSIGN:
      printf("VARASSIGN %s (", stmt->as.assignment.identifier);
      printExprAST(stmt->as.assignment.value);
      printf("))\n"); break;
  }
}

void freeStmtNode(struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED: break;
    case STMT_EXPRESSION:
      freeExprNode(stmt->as.expression.expression); break;
    case STMT_BUILTIN:
      freeExprNode(stmt->as.builtin.parameter); break;
    case STMT_VARDECL: // uhh do i free const char*s; dont think so lol
      freeExprNode(stmt->as.vardecl.value); break;
    case STMT_VARASSIGN:
      freeExprNode(stmt->as.assignment.value); break;
  }

  free(stmt);
}
