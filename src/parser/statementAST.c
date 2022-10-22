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

struct StmtAST* allocNewExpression(struct ExprAST* expr) {
  struct StmtAST ast = newExpression(expr);

  struct StmtAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = ast;

  return ptr;
}

struct StmtAST* allocNewBuiltin(enum BuiltinType type, struct ExprAST* expr) {
  struct StmtAST ast = newBuiltin(type, expr);

  struct StmtAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = ast;

  return ptr;
}

void printStmtAST(const struct StmtAST* stmt) {
  switch(stmt->type) {
    case STMT_UNDEFINED:
      printf("STMT_UNDEFINED\n"); break;
    case STMT_EXPRESSION:
      printf("EXPR ");
      printExprAST(stmt->as.expression.expression);
      printf("\n"); break;
    case STMT_BUILTIN:
      printf("BUILTIN_%d(", stmt->as.builtin.type);
      printExprAST(stmt->as.builtin.parameter);
      printf(")\n"); break;
  }
}
