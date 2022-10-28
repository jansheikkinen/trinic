/* generateStatementAST.c */

#include <stdio.h>

#include "statementAST.h"
#include "generateExpressionAST.h"
#include "generateStatementAST.h"

// statement   = (expression | assignment | vardecl | conditional) "." ;
// builtin     = ("print") "(" expression ")" ;
// vardecl     = type assignment ;
// assignment  = IDENTIFIER "=" (expression | "undefined") ;
// conditional = "if" expression "do" statement*
//               ( "end" | "else" (statement* "end" | conditional) ) ;
// expression = logic ;

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
      } else printf(
          // TODO: Error print function
          "[PARSER ERROR: (%ld, %ld)"
          " Expected right parenthesis to close 'print' statement."
          " Got %s\n",
          ctx->tokens->tokens[ctx->index].row,
          ctx->tokens->tokens[ctx->index].col,
          getTokenName(ctx->tokens->tokens[ctx->index].type));
    } else {
      printf(
          "[PARSER ERROR]: (%ld, %ld)"
          " Expected left parentheses after token 'print'\n",
          ctx->tokens->tokens[ctx->index].row,
          ctx->tokens->tokens[ctx->index].col);
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

static struct StmtAST* genStmtNode(struct ASTContext* ctx) {
  struct StmtAST* stmt = NULL;

  if(MATCH_TOKEN(ctx, TOKEN_PRINT)) stmt = genBuiltinNode(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_LET)) stmt = genVarDeclNode(ctx);
  else stmt = allocNewExpression(generateExpression(ctx));

  if(MATCH_TOKEN(ctx, TOKEN_DOT)) {
    ctx->index += 1;
  } else {
    printf("[PARSER ERROR]: (%ld, %ld) Expected end of statement at %s\n",
        GET_CURRENT_TOKEN(ctx).row, GET_CURRENT_TOKEN(ctx).col,
        getTokenName(GET_CURRENT_TOKEN(ctx).type));
  }

  return stmt;
}

struct StmtAST* generateStatement(struct ASTContext* ctx) {
  struct StmtAST* stmt = genStmtNode(ctx);

  return stmt;
}
