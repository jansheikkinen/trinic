/* generateStatementAST.c */

#include <stdio.h>

#include "statementAST.h"
#include "generateExpressionAST.h"
#include "generateStatementAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) GET_CURRENT_TOKEN(ctx).type == token

static struct StmtAST* genBuiltinNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;

  if(MATCH_TOKEN(ctx, TOKEN_PRINT)) {
    ctx->index += 1;
    if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
      ctx->index += 1;
      struct ExprAST* parameter = generateExpression(ctx);

      if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
        ctx->index += 1;
        stmt = allocNewBuiltin(BUILTIN_PRINT, parameter);
      } else {
        APPEND_ASTERROR(ctx, ASTERR_UNCLOSED_PAREN);
      }
    } else {
      APPEND_ASTERROR(ctx, ASTERR_FUNC_NO_LEFT_PAREN);
    }
  }

  return stmt;
}

static struct StmtAST* genVarDeclNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    // TODO: Type annotations

    if(MATCH_TOKEN(ctx, TOKEN_ASSIGN)) {
      ctx->index += 1;
      struct ExprAST* value = generateExpression(ctx);
      stmt = allocNewVarDecl(identifier, value);
    }
  }

  return stmt;
}

// conditional = "if" expression "do" statement*
//               ( "end" | "else" (statement* "end" | conditional) ) ;
// expression = logic ;


static struct StmtAST* genConditionalNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;
  ctx->index += 1;

  struct ExprAST* condition = generateExpression(ctx);
  if(MATCH_TOKEN(ctx, TOKEN_DO)) {
    ctx->index += 1;
    struct StmtList* body = malloc(sizeof(*body));
    *body = newStmtList();

    enum ConditionalElseTypes type = CONDELSE_NONE;
    union ConditionalElse elseBranch;

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_END)) {
        ctx->index += 1;
        stmt = allocNewConditional(condition, body, type, elseBranch);
      } else if(MATCH_TOKEN(ctx, TOKEN_ELSE)) {
        ctx->index += 1;
        if(MATCH_TOKEN(ctx, TOKEN_IF)) {
          ctx->index += 1;
          type = CONDELSE_ELSEIF;
          elseBranch.elseif = genConditionalNode(ctx);
        } else {
          type = CONDELSE_ELSE;
          elseBranch.elseBody = malloc(sizeof(*elseBranch.elseBody));
          *elseBranch.elseBody = newStmtList();

          while(ctx->index < ctx->tokens->length) {
            if(MATCH_TOKEN(ctx, TOKEN_END)) {
              ctx->index += 1;
              stmt = allocNewConditional(condition, body, type, elseBranch);
            } else {
              appendToStmtList(elseBranch.elseBody, generateStatement(ctx));
            }
          }
        }
      } else {
        appendToStmtList(body, generateStatement(ctx));
      }
    }

    // Missing end token?
    if(ctx->index >= ctx->tokens->length) {
      APPEND_ASTERROR(ctx, ASTERR_UNTERMINATED_IF);
      freeStmtList(body);
      if(type == CONDELSE_ELSE) freeStmtList(elseBranch.elseBody);
    }

  }
  return stmt;
}

static struct StmtAST* genStmtNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;

  if(MATCH_TOKEN(ctx, TOKEN_IF)) {
    stmt = genConditionalNode(ctx);
  } else {
    // Everything in this branch must end with a .
    if(MATCH_TOKEN(ctx, TOKEN_PRINT)) stmt = genBuiltinNode(ctx);
    else if(MATCH_TOKEN(ctx, TOKEN_LET)) stmt = genVarDeclNode(ctx);
    else stmt = allocNewExpression(generateExpression(ctx));

    if(MATCH_TOKEN(ctx, TOKEN_DOT)) ctx->index += 1;
    else APPEND_ASTERROR(ctx, ASTERR_STMT_END);
  }

  return stmt;
}

struct StmtAST* generateStatement(struct ASTContext* ctx) {
  struct StmtAST* stmt = genStmtNode(ctx);

  return stmt;
}
