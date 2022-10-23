#ifndef STATEMENTAST_H
#define STATEMENTAST_H

#include "expressionAST.h"

// 1 + 1.
struct ExprStmt {
  struct ExprAST* expression;
};

// TODO: Built-ins should be deleted later
enum BuiltinType {
  BUILTIN_PRINT,
};

// Built-ins only take a single argument
struct BuiltinStmt {
  enum BuiltinType type;
  struct ExprAST* parameter;
};

enum StmtASTType {
  STMT_UNDEFINED,
  STMT_EXPRESSION,
  STMT_BUILTIN,
};

struct StmtAST {
  enum StmtASTType type;
  union {
    struct ExprStmt expression;
    struct BuiltinStmt builtin;
  } as;
};

struct StmtAST* allocNewExpression(struct ExprAST*);
struct StmtAST* allocNewBuiltin(enum BuiltinType, struct ExprAST*);

void printStmtAST(const struct StmtAST*);

void freeStmtNode(struct StmtAST* stmt);

#endif
