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
  EXPR_CALL,
  EXPR_GET,
  EXPR_ARRAY_INDEX,
  EXPR_ARRAY_INIT,
  EXPR_FUNCTION,
  EXPR_CAST,
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

struct GetExpression {
  bool isPointer;
  const char* name;
  struct ExprAST* expression;
};

struct GroupingExpression {
  struct ExprAST* expression;
};

struct ArrayIndexExpression {
  struct ExprAST* index;
  struct ExprAST* identifier;
};

struct ArrayInitExpression {
  struct ArgAST* args;
};

struct FunctionExpression {
  struct DeclarationAST* function;
};

struct CastExpression {
  struct ExprAST* expr;
  struct TypeAST* type;
};

struct ExprAST {
  enum ExprASTType type;
  union {
    struct LiteralExpression literal;
    struct UnaryExpression   unary;
    struct BinaryExpression  binary;
    struct VariableExpression variable;
    struct CallExpression call;
    struct GetExpression get;
    struct GroupingExpression grouping;
    struct ArrayIndexExpression arrindex;
    struct ArrayInitExpression arrinit;
    struct FunctionExpression function;
    struct CastExpression cast;
  } as;
};

struct ExprAST* allocNewLiteral(enum LiteralType, union LiteralValue);
struct ExprAST* allocNewUnary(enum TokenType, struct ExprAST*);
struct ExprAST* allocNewBinary(enum TokenType, struct ExprAST*, struct ExprAST*);
struct ExprAST* allocNewVariable(const char* identifier);
struct ExprAST* allocNewCall(struct ExprAST*, struct ArgAST*);
struct ExprAST* allocNewGet(bool, const char*, struct ExprAST*);
struct ExprAST* allocNewGrouping(struct ExprAST*);
struct ExprAST* allocNewArrayIndex(struct ExprAST*, struct ExprAST*);
struct ExprAST* allocNewArrayInit(struct ArgAST*);
struct ExprAST* allocNewFunctionExpr(struct DeclarationAST*);
struct ExprAST* allocNewCastExpr(struct ExprAST*, struct TypeAST*);
struct ExprAST* allocNewUndefined(void);

#define ALLOC_NEW_LITERAL(type, vtype, value) \
  allocNewLiteral(type, (union LiteralValue){ .vtype = value })

void printExprAST(const struct ExprAST*, size_t);

void freeExprNode(struct ExprAST* expr);

#endif
