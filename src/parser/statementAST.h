#ifndef STATEMENTAST_H
#define STATEMENTAST_H

#include "stmtlist.h"
#include "expressionAST.h"
#include "argumentAST.h"

// 1 + 1.
struct ExprStmt {
  struct ExprAST* expression;
};

enum BuiltinType {
  BUILTIN_UNDEFINED,
  BUILTIN_PRINT, // TODO: `print` should be part of std, not builtin
  BUILTIN_RETURN,
  BUILTIN_BREAK,
  BUILTIN_CONTINUE,
};

extern const char* builtinNames[];

// Built-ins only take a single argument
struct BuiltinStmt {
  enum BuiltinType type;
  struct ExprAST* parameter;
};

// let x: int32 = 0.
struct VarDeclStmt {
  struct ArgAST* lvalue;
  struct ArgAST* rvalue;
};

// x = 0.
struct VarAssignStmt {
  enum TokenType op;
  struct ArgAST* lvalue;
  struct ArgAST* rvalue;
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

// loop print(x). end
struct LoopStmt {
  struct StmtList* body;
};

// while x < 3 do x += 1. end
struct WhileStmt {
  struct ExprAST* condition;
  struct StmtList* body;
};

// for x: int8 in { 1, 2, 3 } do z; end
struct ForStmt {
  struct ArgAST* itervar;
  struct ExprAST* iterator;
  struct StmtList* body;
};

struct MatchStmt {
  struct ExprAST* expr;
  struct ArgAST* body;
};

enum StmtASTType {
  STMT_UNDEFINED,
  STMT_EXPRESSION,
  STMT_BUILTIN,
  STMT_VARDECL,
  STMT_VARASSIGN,
  STMT_CONDITIONAL,
  STMT_LOOP,
  STMT_WHILE,
  STMT_FOR,
  STMT_MATCH,
};

struct StmtAST {
  enum StmtASTType type;
  union {
    struct ExprStmt expression;
    struct BuiltinStmt builtin;
    struct VarDeclStmt vardecl;
    struct VarAssignStmt assignment;
    struct ConditionalStmt conditional;
    struct LoopStmt loop;
    struct WhileStmt whileloop;
    struct ForStmt forloop;
    struct MatchStmt match;
  } as;
};

struct StmtAST* allocNewExpression(struct ExprAST*);
struct StmtAST* allocNewBuiltin(enum BuiltinType, struct ExprAST*);
struct StmtAST* allocNewVarDecl(struct ArgAST*, struct ArgAST*);
struct StmtAST* allocNewAssign(enum TokenType, struct ArgAST*, struct ArgAST*);
struct StmtAST* allocNewConditional(struct ExprAST*, struct StmtList*,
    enum ConditionalElseTypes, union ConditionalElse);
struct StmtAST* allocNewLoop(struct StmtList*);
struct StmtAST* allocNewWhile(struct ExprAST*, struct StmtList*);
struct StmtAST* allocNewFor(struct ArgAST*, struct ExprAST*, struct StmtList*);
struct StmtAST* allocNewMatch(struct ExprAST*, struct ArgAST*);

void printStmtAST(const struct StmtAST*, size_t);

void freeStmtNode(struct StmtAST* stmt);

#endif
