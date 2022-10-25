/* expressionAST.c */

/* Related to functions needed to create expression AST nodes */

#include <stdio.h>

#include "expressionAST.h"


static struct ExprAST newLiteral(enum LiteralType type, void* value) {
  struct ExprAST expr;

  struct LiteralExpression literal;
  literal.type = type;
  literal.as = (union LiteralValue){ value };

  expr.type = EXPR_LITERAL;
  expr.as.literal = literal;
  return expr;
}

static struct ExprAST newUnary(enum TokenType operation, struct ExprAST* operand) {
  struct ExprAST expr;

  struct UnaryExpression unary;
  unary.operation = operation;
  unary.operand = operand;

  expr.type = EXPR_UNARY;
  expr.as.unary = unary;
  return expr;
}

static struct ExprAST newBinary(enum TokenType operation,
  struct ExprAST* left, struct ExprAST* right) {
  struct ExprAST expr;

  struct BinaryExpression binary;
  binary.operation = operation;
  binary.left = left;
  binary.right = right;

  expr.type = EXPR_BINARY;
  expr.as.binary = binary;
  return expr;
}

struct ExprAST* allocNewLiteral(enum LiteralType type, void* value) {
  struct ExprAST ast = newLiteral(type, value);

  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = ast;

  return ptr;
}

struct ExprAST* allocNewUnary(enum TokenType operation,
    struct ExprAST* operand) {
  struct ExprAST ast = newUnary(operation, operand);

  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = ast;

  return ptr;
}

struct ExprAST* allocNewBinary(enum TokenType operation,
    struct ExprAST* left, struct ExprAST* right) {
  struct ExprAST ast = newBinary(operation, left, right);

  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = ast;

  return ptr;
}

void printExprAST(const struct ExprAST* ast) {
  switch(ast->type) {
    case EXPR_UNDEFINED: printf("EXPR_UNDEFINED"); break;
    case EXPR_LITERAL:
      switch(ast->as.literal.type) {
        case LIT_UNDEFINED: printf("LIT_UNDEFINED"); break;
        case LIT_IDENTIFIER: printf("%s", ast->as.literal.as.identifier); break;
        case LIT_STRING: printf("\"%s\"", ast->as.literal.as.string);     break;
        case LIT_CHAR: printf("'%c'",     ast->as.literal.as.character);  break;
        case LIT_INTEGER: printf("%ld",   ast->as.literal.as.integer);    break;
        case LIT_UINTEGER: printf("%lu",  ast->as.literal.as.uinteger);   break;
        case LIT_FLOAT: printf("%lf",     ast->as.literal.as.floating);   break;
        case LIT_BOOL:
          printf("%s", ast->as.literal.as.boolean ? "true" : "false"); break;
      } break;
    case EXPR_UNARY:
      printf("(%s ", getTokenName(ast->as.unary.operation));
      printExprAST(ast->as.unary.operand);
      printf(")"); break;
    case EXPR_BINARY:
      printf("(%s ", getTokenName(ast->as.binary.operation));
      printExprAST(ast->as.binary.left);
      printf(" ");
      printExprAST(ast->as.binary.right);
      printf(")"); break;
  }
}

void freeExprNode(struct ExprAST* expr) {
  switch(expr->type) {
    case EXPR_UNDEFINED: break;
    case EXPR_LITERAL:
      switch(expr->as.literal.type) {
        case LIT_IDENTIFIER:
          free(expr->as.literal.as.identifier); break;
        case LIT_STRING:
          free(expr->as.literal.as.string); break;
        default: break;
      } break;
    case EXPR_UNARY:
      freeExprNode(expr->as.unary.operand); break;
    case EXPR_BINARY:
      freeExprNode(expr->as.binary.left);
      freeExprNode(expr->as.binary.right);
      break;
  }

  free(expr);
}
