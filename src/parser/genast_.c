/* genast.c */

/* Generation of an AST */

#include <stdio.h>

#include "../debug/debug.h"
#include "genast.h"

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

// cursed C functional programming moment
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
#ifdef AST_DEBUG
  printf("[AST]: Generating AST...\n");
#endif

  struct ASTContext* ctx = newASTContext(tokens);
  struct AST* ast = expression(ctx);

  freeASTContext(ctx);
  return ast;
}

#undef GET_TOKEN
#undef MATCH
