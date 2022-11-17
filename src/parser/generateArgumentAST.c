/* generateArgumentAST.c */

#include "../error/error.h"
#include "../util/arraylist.h"
#include "argumentAST.h"
#include "generateTypeAST.h"
#include "generateExpressionAST.h"

// Differentiating these two in a single function as I have in expression or
// statement generation is actually a bit more involved to implement than I
// would like. Luckily, these two can never appear in the same spot, so I can
// just call the relevant function when it is needed. :)

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) (GET_CURRENT_TOKEN(ctx).type == token)

struct ArgAST* generateExpressionArgumentsWithFirst(struct ASTContext* ctx,
    struct ExprAST* arg) {
  struct ArgAST* ast = NULL;

  struct ExprList* args = malloc(sizeof(*args));
  NEW_ARRAYLIST(args);
  APPEND_ARRAYLIST(args, arg);

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;
      APPEND_ARRAYLIST(args, generateExpression(ctx));
    } else return allocNewExprArgList(args);
  }

  return ast;
}

struct ArgAST* generateExpressionArguments(struct ASTContext* ctx) {
  struct ArgAST* ast = NULL;

  struct ExprAST* expr = generateExpression(ctx);
  struct ExprList* args = malloc(sizeof(*args));
  NEW_ARRAYLIST(args);
  APPEND_ARRAYLIST(args, expr);

  while(ctx->index < ctx->tokens->length) {
    if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
      ctx->index += 1;
      APPEND_ARRAYLIST(args, generateExpression(ctx));
    } else return allocNewExprArgList(args);
  }

  if(ctx->index >= ctx->tokens->length) {
    APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_EXPR_ARGS);
    FREE_SELF_AND_MEMBERS(args, freeExprNode);
  }

  return ast;
}

static struct IdentifierArg* generateIdentifierArg(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_COLON)) {
      ctx->index += 1;

      struct TypeAST* type = generateType(ctx);
      return allocNewIdentifierArg(identifier, type);
    } else return allocNewIdentifierArg(identifier, NULL);
  } else {
    APPEND_ASTERROR(ctx, ASTERR_MISSING_IDENTIFIER_IDARGS);
  }
  return NULL;
}

struct ArgAST* generateIdentifierArguments(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    struct ArgAST* ast = allocNewIdentifierArgList();

    struct IdentifierArg* arg = generateIdentifierArg(ctx);
    APPEND_ARRAYLIST(&ast->as.idargs, arg);

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
        ctx->index += 1;

        arg = generateIdentifierArg(ctx);
        APPEND_ARRAYLIST(&ast->as.idargs, arg);
      } else return ast;
   }

    if(ctx->index >= ctx->tokens->length) {
      APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_IDENTIFIER_ARGS);
      freeArgAST(ast);
    }
  }

  return NULL;
}

static struct AssignArg* generateAssignArg(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_EQUAL)) {
      ctx->index += 1;

      if(MATCH_TOKEN(ctx, TOKEN_INTEGER_LITERAL)) {
        long value = strtol(GET_CURRENT_TOKEN(ctx).literal, NULL, 10);
        return allocNewAssignArg(identifier, value);
      }
    }
  }

  return NULL;
}

struct ArgAST* generateAssignmentArguments(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    struct ArgAST* ast = allocNewAssignArgList();

    struct AssignArg* arg = generateAssignArg(ctx);
    APPEND_ARRAYLIST(&ast->as.assignargs, arg);

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_COMMA)) {
        ctx->index += 1;
        arg = generateAssignArg(ctx);
        APPEND_ARRAYLIST(&ast->as.assignargs, arg);
      } else return ast;
    }
  }

  return NULL;
}
