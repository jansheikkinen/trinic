#ifndef STATEMENTAST_H
#define STATEMENTAST_H

#include "stmtlist.h"
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

enum ConditionalElseTypes {
  CONDELSE_UNDEFINED,
  CONDELSE_NONE,
  CONDELSE_ELSE,
  CONDELSE_ELSEIF,
};
union ConditionalElse {
  struct StmtList* elseBody;
  struct StmtAST* elseif;
};

/* if x do
 *   a
 * else if y do
 *   b
 * else
 *   z
 * end */
struct ConditionalStmt {
  struct ExprAST* condition;
  struct StmtList* body;
  enum ConditionalElseTypes type;
  union ConditionalElse elseBranch;
};

enum StmtASTType {
  STMT_UNDEFINED,
  STMT_EXPRESSION,
  STMT_BUILTIN,
  STMT_VARDECL,
  STMT_VARASSIGN,
  STMT_CONDITIONAL,
};

struct StmtAST {
  enum StmtASTType type;
  union {
    struct ExprStmt expression;
    struct BuiltinStmt builtin;
    struct VarDeclStmt vardecl;
    struct VarAssignStmt assignment;
    struct ConditionalStmt conditional;
  } as;
};

struct StmtAST* allocNewExpression(struct ExprAST*);
struct StmtAST* allocNewBuiltin(enum BuiltinType, struct ExprAST*);
struct StmtAST* allocNewVarDecl(const char*, struct ExprAST*);
struct StmtAST* allocNewAssign(const char*, struct ExprAST*);
struct StmtAST* allocNewConditional(struct ExprAST*, struct StmtList*,
    enum ConditionalElseTypes, union ConditionalElse);

void printStmtAST(const struct StmtAST*);

void freeStmtNode(struct StmtAST* stmt);

#endif
