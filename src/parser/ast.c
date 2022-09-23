/* ast.c */

/* The shit that's needed to create and use an AST */

#include <stdio.h>
#include <stdlib.h>

#include "../util/strutil.h"
#include "ast.h"

struct AST* newASTNode(struct AST ast) {
  struct AST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = ast;
  return ptr;
}

void freeASTNode(struct AST* ast) {
  switch(ast->type) {
    case AST_UNDEFINED:
    case AST_LITERAL:
      free(ast); break;
    case AST_UNARY:
      freeASTNode(ast->as.unaryExpression.operand);
      free(ast);
      break;
    case AST_BINARY:
      freeASTNode(ast->as.binaryExpression.left);
      freeASTNode(ast->as.binaryExpression.right);
      free(ast);
      break;
  }
}

void printASTNode(const struct AST* ast) {
  if(ast == NULL) { printf("NULL NODE"); return; }
  switch(ast->type) {
    case AST_UNDEFINED: printf("UNDEFINED\n"); break;
    case AST_LITERAL:
      switch(ast->as.literal.type) {
        case LIT_UNDEFINED: printf("UNDEFINED"); break;
        case LIT_IDENTIFIER: printf("%s", ast->as.literal.as.identifier); break;
        case LIT_STRING: printf("\"%s\"", ast->as.literal.as.string);     break;
        case LIT_INT64: printf("%ld",     ast->as.literal.as.integer);    break;
        case LIT_UINT64: printf("%lu",    ast->as.literal.as.uinteger);   break;
        case LIT_FLOAT64: printf("%lf",   ast->as.literal.as.floating);   break;
        case LIT_BOOL:
          printf("%s", ast->as.literal.as.boolean ? "true" : "false");    break;
      } break;
    case AST_UNARY: {
      char* type = getTokenName(ast->as.unaryExpression.type);
      char* typelower = calloc(strlen(type), 1);
      strncpy(typelower, type, strlen(type));
      printf("(%s ", strlower(typelower));
      free(typelower);
      printASTNode(ast->as.unaryExpression.operand);
      printf(")"); break;
    } case AST_BINARY: {
      char* type = getTokenName(ast->as.binaryExpression.type);
      char* typelower = calloc(strlen(type), 1);
      strncpy(typelower, type, strlen(type));
      printf("(%s ", strlower(typelower));
      free(typelower);
      printASTNode(ast->as.binaryExpression.left);
      printf(" ");
      printASTNode(ast->as.binaryExpression.right);
      printf(")");
      break;
    }
  }
}

struct AST* newLiteralNode(enum LiteralType type, union LiteralValue literal) {
  struct AST lit;
  lit.type = AST_LITERAL;
  lit.as.literal.type = type;
  lit.as.literal.as = literal;

  return newASTNode(lit);
}

struct AST* newUnaryNode(enum TokenType operator, struct AST* operand) {
  struct AST ast;
  ast.type = AST_UNARY;
  ast.as.unaryExpression.type = operator;
  ast.as.unaryExpression.operand = operand;

  return newASTNode(ast);
}

struct AST* newBinaryNode(enum TokenType operator,
    struct AST* left, struct AST* right) {
  struct AST ast;
  ast.type = AST_BINARY;
  ast.as.binaryExpression.type = operator;
  ast.as.binaryExpression.left = left;
  ast.as.binaryExpression.right = right;

  return newASTNode(ast);
}
