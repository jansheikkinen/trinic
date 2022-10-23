/* generateAST.c */

#include "ASTContext.h"
#include "stmtlist.h"
#include "generateStatementAST.h"
#include "generateAST.h"

struct StmtList* generateAST(struct TokenArray* tokens) {
  struct ASTContext* ctx = malloc(sizeof(*ctx));
  *ctx = newASTContext(tokens);

  struct StmtList* stmts = malloc(sizeof(*stmts));
  *stmts = newStmtList();

  while(ctx->index <= ctx->tokens->length) {
    appendToStmtList(stmts, generateStatement(ctx));
  }

  free(ctx);
  return stmts;
}
