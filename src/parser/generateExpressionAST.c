/* generateExpressionAST.c */

#include <stdio.h>

#include "generateExpressionAST.h"
#include "ASTContext.h"
#include "token.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) GET_CURRENT_TOKEN(ctx).type == token

// back in my day, *this* is what functional programming looked like
#define generateBinaryNode(nodeName, prevNode, condition)   \
  static struct ExprAST* nodeName(struct ASTContext* ctx) { \
    struct ExprAST* left = prevNode(ctx);                   \
    while(condition) {                                      \
      enum TokenType op = GET_CURRENT_TOKEN(ctx).type;      \
      ctx->index += 1;                                      \
      struct ExprAST* right = prevNode(ctx);                \
      left = allocNewBinary(op, left, right);               \
    }                                                       \
    return left;                                            \
  }

static struct ExprAST* genPrimaryNode(struct ASTContext* ctx) {
  struct ExprAST* expr = NULL;

  if(MATCH_TOKEN(ctx, TOKEN_TRUE)) {
    expr = allocNewLiteral(LIT_BOOL, (void*)1);
  } else if(MATCH_TOKEN(ctx, TOKEN_FALSE)) {
    expr = allocNewLiteral(LIT_BOOL, 0);

  } else if(MATCH_TOKEN(ctx, TOKEN_INTEGER_LITERAL)) {
    expr = allocNewLiteral(LIT_INTEGER, (void*)
        strtol(GET_CURRENT_TOKEN(ctx).literal, NULL, 10));
  } else if(MATCH_TOKEN(ctx, TOKEN_FLOAT_LITERAL)) {
    expr = allocNewLiteral(LIT_FLOAT, (void*)(long) // i regret everything
        strtod(GET_CURRENT_TOKEN(ctx).literal, NULL));

  } else if(MATCH_TOKEN(ctx, TOKEN_CHAR_LITERAL)) {
    char ch = *GET_CURRENT_TOKEN(ctx).literal;
    expr = allocNewLiteral(LIT_CHAR, (void*)(long)ch); // $10 says this'll cause
  } else if(MATCH_TOKEN(ctx, TOKEN_STRING_LITERAL)) {  // many problems later
    expr = allocNewLiteral(LIT_STRING, GET_CURRENT_TOKEN(ctx).literal);
  } else if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    // saving the following in case its needed later
    // expr = allocNewLiteral(LIT_IDENTIFIER, GET_CURRENT_TOKEN(ctx).literal);
    expr = allocNewVariable(GET_CURRENT_TOKEN(ctx).literal);

  } else if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
    ctx->index += 1;
    struct ExprAST* exprast = generateExpression(ctx);

    if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
      expr = allocNewGrouping(exprast);
    } else
      printf("[PARSER ERROR]: (%ld, %ld) "
          "Expected closing parenthesis in expression; got %s\n",
          GET_CURRENT_TOKEN(ctx).row, GET_CURRENT_TOKEN(ctx).col,
          getTokenName(GET_CURRENT_TOKEN(ctx).type));

  } else {
    printf("[PARSER ERROR]: (%ld, %ld) Expected expression at token %s\n",
        GET_CURRENT_TOKEN(ctx).row, GET_CURRENT_TOKEN(ctx).col,
        getTokenName(GET_CURRENT_TOKEN(ctx).type));
  }

  ctx->index += 1;
  return expr;
}

static struct ExprAST* genUnaryNode(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_TILDE)
      || MATCH_TOKEN(ctx, TOKEN_BANG)
      || MATCH_TOKEN(ctx, TOKEN_NOT)
      || MATCH_TOKEN(ctx, TOKEN_MINUS)) {
    enum TokenType operator = GET_CURRENT_TOKEN(ctx).type;
    ctx->index += 1;
    struct ExprAST* operand = genUnaryNode(ctx);

    return allocNewUnary(operator, operand);
  }

  return genPrimaryNode(ctx);
}

generateBinaryNode(genFactorNode, genUnaryNode,
  MATCH_TOKEN(ctx, TOKEN_SLASH) || MATCH_TOKEN(ctx, TOKEN_STAR))

generateBinaryNode(genTermNode, genFactorNode,
  MATCH_TOKEN(ctx, TOKEN_MINUS) || MATCH_TOKEN(ctx, TOKEN_ADD))

generateBinaryNode(genBitwiseNode, genTermNode,
  MATCH_TOKEN(ctx, TOKEN_BIT_AND) || MATCH_TOKEN(ctx, TOKEN_BIT_OR)
  || MATCH_TOKEN(ctx, TOKEN_BIT_XOR)
  || MATCH_TOKEN(ctx, TOKEN_BIT_LEFT) || MATCH_TOKEN(ctx, TOKEN_BIT_RIGHT))

generateBinaryNode(genComparisonNode, genBitwiseNode,
  MATCH_TOKEN(ctx, TOKEN_LEFT_DIAMOND) || MATCH_TOKEN(ctx, TOKEN_RIGHT_DIAMOND)
  || MATCH_TOKEN(ctx, TOKEN_LESS_EQ) || MATCH_TOKEN(ctx, TOKEN_GREATER_EQ))

generateBinaryNode(genEqualityNode, genComparisonNode,
  MATCH_TOKEN(ctx, TOKEN_NOT_EQ) || MATCH_TOKEN(ctx, TOKEN_EQUAL))

generateBinaryNode(genLogicNode, genEqualityNode,
  MATCH_TOKEN(ctx, TOKEN_AND) || MATCH_TOKEN(ctx, TOKEN_OR))

#undef generateBinaryNode

struct ExprAST* generateExpression(struct ASTContext* ctx) {
  struct ExprAST* ast = genLogicNode(ctx);

  return ast;
}
