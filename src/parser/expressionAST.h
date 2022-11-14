#ifndef EXPRESSIONAST_H
#define EXPRESSIONAST_H

#include <stdbool.h>

#include "token.h"

struct ExprAST;

enum ExprASTType {
  EXPR_UNDEFINED,
  EXPR_LITERAL,
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_VARIABLE,
  EXPR_GROUPING,
  EXPR_CALL
};

enum LiteralType {
  LIT_UNDEFINED,
  LIT_IDENTIFIER,
  LIT_CHAR,
  LIT_STRING,
  LIT_INTEGER,
  LIT_UINTEGER,
  LIT_FLOAT,
  LIT_BOOL,
};

union LiteralValue {
  char*         identifier;
  char*         string;
  char          character;
  double        floating;
  long          integer;
  unsigned long uinteger;
  bool          boolean;
};

struct LiteralExpression {
  enum LiteralType type;
  union LiteralValue as;
};

struct UnaryExpression {
  enum TokenType operation;
  struct ExprAST* operand;
};

struct BinaryExpression {
  enum TokenType operation;
  struct ExprAST* left;
  struct ExprAST* right;
};

struct VariableExpression {
  const char* identifier;
};

struct CallExpression {
  struct ExprAST* callee;
  struct ArgAST* args;
};

struct GroupingExpression {
  struct ExprAST* expression;
};

struct ExprAST {
  enum ExprASTType type;
  union {
    struct LiteralExpression literal;
    struct UnaryExpression   unary;
    struct BinaryExpression  binary;
    struct VariableExpression variable;
    struct CallExpression call;
    struct GroupingExpression grouping;
  } as;
};

struct ExprAST* allocNewLiteral(enum LiteralType, union LiteralValue);
struct ExprAST* allocNewUnary(enum TokenType, struct ExprAST*);
struct ExprAST* allocNewBinary(enum TokenType, struct ExprAST*, struct ExprAST*);
struct ExprAST* allocNewVariable(const char* identifier);
struct ExprAST* allocNewCall(struct ExprAST*, struct ArgAST*);
struct ExprAST* allocNewGrouping(struct ExprAST*);

#define ALLOC_NEW_LITERAL(type, vtype, value) \
  allocNewLiteral(type, (union LiteralValue){ .vtype = value })

void printExprAST(const struct ExprAST*);

void freeExprNode(struct ExprAST* expr);

#endif
