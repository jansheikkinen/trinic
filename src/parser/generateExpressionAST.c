/* generateExpressionAST.c */

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

struct ExprAST* genPrimaryNode(struct ASTContext* ctx);
struct ExprAST* genUnaryNode(struct ASTContext* ctx);

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
