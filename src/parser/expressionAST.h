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

struct ExprAST {
  enum ExprASTType type;
  union {
    struct LiteralExpression literal;
    struct UnaryExpression   unary;
    struct BinaryExpression  binary;
  } as;
};

struct ExprAST* allocNewLiteral(enum LiteralType, void*);
struct ExprAST* allocNewUnary(enum TokenType, struct ExprAST*);
struct ExprAST* allocNewBinary(enum TokenType, struct ExprAST*, struct ExprAST*);

void printExprAST(const struct ExprAST*);

void freeExprNode(struct ExprAST* expr);

#endif
