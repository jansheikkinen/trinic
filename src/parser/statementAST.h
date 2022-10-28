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

// let x: int32 = 0.
// TODO: Add types lol
struct VarDeclStmt {
  // type information
  const char* identifier;
  struct ExprAST* value;
};

// x = 0.
struct VarAssignStmt {
  const char* identifier;
  struct ExprAST* value;
};

enum StmtASTType {
  STMT_UNDEFINED,
  STMT_EXPRESSION,
  STMT_BUILTIN,
  STMT_VARDECL,
  STMT_VARASSIGN
};

struct StmtAST {
  enum StmtASTType type;
  union {
    struct ExprStmt expression;
    struct BuiltinStmt builtin;
    struct VarDeclStmt vardecl;
    struct VarAssignStmt assignment;
  } as;
};

struct StmtAST* allocNewExpression(struct ExprAST*);
struct StmtAST* allocNewBuiltin(enum BuiltinType, struct ExprAST*);
struct StmtAST* allocNewVarDecl(const char*, struct ExprAST*);
struct StmtAST* allocNewAssign(const char*, struct ExprAST*);

void printStmtAST(const struct StmtAST*);

void freeStmtNode(struct StmtAST* stmt);

#endif
