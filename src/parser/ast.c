/* ast.c */

/* Generation of an abstract syntax tree from an array of tokens
 * Honestly, this step could have been skipped, but supposedly this
 * is a fairly common step in most modern compilers. I'm assuming that
 * its because of how easy they are to work with. For example, compiling
 * an AST to assembly / bytecode just consists of walking the tree and printing
 * out each node as they come. */

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "token.h"

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
        case LIT_IDENTIFIER: printf("%s", ast->as.literal.as.identifier); break;
        case LIT_STRING: printf("%s", ast->as.literal.as.string); break;
        case LIT_INT64: printf("%ld", ast->as.literal.as.integer); break;
        case LIT_UINT64: printf("%lu", ast->as.literal.as.uinteger); break;
        case LIT_FLOAT64: printf("%lf", ast->as.literal.as.floating); break;
        case LIT_BOOL:
          printf("%s", ast->as.literal.as.boolean ? "true" : "false"); break;
      } break;
    case AST_UNARY:
      printf("(%s ", getTokenName(ast->as.unaryExpression.type));
      printASTNode(ast->as.unaryExpression.operand);
      printf(")"); break;
    case AST_BINARY:
      printf("(%s ", getTokenName(ast->as.binaryExpression.type));
      printASTNode(ast->as.binaryExpression.left);
      printf(" ");
      printASTNode(ast->as.binaryExpression.right);
      printf(")");
      break;
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

struct ASTContext* newASTContext(struct TokenArray* tokens) {
  struct ASTContext* ctx = malloc(sizeof(*ctx));
  ctx->tokens = tokens;
  ctx->index = 0;

  return ctx;
}

void freeASTContext(struct ASTContext* ctx) {
  freeTokenArray(ctx->tokens);
  free(ctx);
}

#define GET_TOKEN(ctx) ctx->tokens->tokens[ctx->index]

#define MATCH(ctx, ttype) GET_TOKEN(ctx).type == ttype

struct AST* expression(struct ASTContext* ctx);

struct AST* primaryExpression(struct ASTContext* ctx) {
  struct AST* ast = NULL;
  if(MATCH(ctx, TOKEN_TRUE))
    ast = newLiteralNode(LIT_BOOL, NEW_LITERAL_VALUE(boolean, 1));
  else if(MATCH(ctx, TOKEN_FALSE))
    ast = newLiteralNode(LIT_BOOL, NEW_LITERAL_VALUE(boolean, 0));

  else if(MATCH(ctx, TOKEN_INTEGER_LITERAL))
    ast = newLiteralNode(LIT_INT64,
        NEW_LITERAL_VALUE(integer, strtol(GET_TOKEN(ctx).literal, NULL, 10)));
  else if(MATCH(ctx, TOKEN_FLOAT_LITERAL))
    ast = newLiteralNode(LIT_FLOAT64,
        NEW_LITERAL_VALUE(floating, strtod(GET_TOKEN(ctx).literal, NULL)));
  else if(MATCH(ctx, TOKEN_STRING_LITERAL))
    ast = newLiteralNode(LIT_STRING,
        NEW_LITERAL_VALUE(string, GET_TOKEN(ctx).literal));

  else if(MATCH(ctx, TOKEN_LEFT_PAREN)) {
    ctx->index += 1;
    ast = expression(ctx);
    if(!(MATCH(ctx, TOKEN_RIGHT_PAREN)))
      printf("[PARSER ERROR]: (%ld, %ld) Unmatched '('\n",
          GET_TOKEN(ctx).row, GET_TOKEN(ctx).col);

  } else
    printf("[PARSER ERROR]: (%ld, %ld) Expected expression at token %s\n",
        GET_TOKEN(ctx).row, GET_TOKEN(ctx).col,
        getTokenName(GET_TOKEN(ctx).type));


  ctx->index += 1;
  return ast;
}

struct AST* unaryExpression(struct ASTContext* ctx) {
  if(MATCH(ctx, TOKEN_TILDE)
      || MATCH(ctx, TOKEN_BANG)
      || MATCH(ctx, TOKEN_NOT)
      || MATCH(ctx, TOKEN_MINUS)) {
    enum TokenType operator = GET_TOKEN(ctx).type;
    ctx->index += 1;
    struct AST* operand = unaryExpression(ctx);

    return newUnaryNode(operator, operand);
  }

  return primaryExpression(ctx);
}

#define binaryExpression(name, prev, cond)            \
  struct AST* name(struct ASTContext* ctx) {          \
    struct AST* left = prev(ctx);                     \
    while(cond) {                                     \
      enum TokenType operator = GET_TOKEN(ctx).type;  \
      ctx-> index += 1;                               \
      struct AST* right = prev(ctx);                  \
      left = newBinaryNode(operator, left, right);    \
    }                                                 \
    return left;                                      \
  }

binaryExpression(factorExpression, unaryExpression,
    MATCH(ctx, TOKEN_SLASH) || MATCH(ctx, TOKEN_STAR))

binaryExpression(termExpression, factorExpression,
    MATCH(ctx, TOKEN_MINUS) || MATCH(ctx, TOKEN_ADD))

binaryExpression(bitwiseExpression, termExpression,
    MATCH(ctx, TOKEN_BIT_AND) || MATCH(ctx, TOKEN_BIT_OR)
    || MATCH(ctx, TOKEN_BIT_XOR)
    || MATCH(ctx, TOKEN_BIT_LEFT) || MATCH(ctx, TOKEN_BIT_RIGHT))

binaryExpression(comparisonExpression, bitwiseExpression,
    MATCH(ctx, TOKEN_LEFT_DIAMOND) || MATCH(ctx, TOKEN_RIGHT_DIAMOND)
    || MATCH(ctx, TOKEN_LESS_EQ) || MATCH(ctx, TOKEN_GREATER_EQ))

binaryExpression(equalityExpression, comparisonExpression,
    MATCH(ctx, TOKEN_NOT_EQ) || MATCH(ctx, TOKEN_EQUAL))

binaryExpression(logicExpression, equalityExpression,
    MATCH(ctx, TOKEN_AND) || MATCH(ctx, TOKEN_OR))

struct AST* expression(struct ASTContext* ctx) {
  return logicExpression(ctx);
}

struct AST* generateAST(struct TokenArray* tokens) {
  struct ASTContext* ctx = newASTContext(tokens);

  struct AST* ast = expression(ctx);

  freeASTContext(ctx);
  return ast;
}
