/* generateAST.c */

#include <stdio.h>

#include "ASTContext.h"
#include "stmtlist.h"
#include "generateStatementAST.h"
#include "generateAST.h"

struct StmtList* generateAST(const char* filename, struct TokenArray* tokens) {
  struct ASTContext* ctx = malloc(sizeof(*ctx));
  *ctx = newASTContext(tokens);

  struct StmtList* stmts = malloc(sizeof(*stmts));
  *stmts = newStmtList();

  while(ctx->index < ctx->tokens->length) {
    appendToStmtList(stmts, generateStatement(ctx));
  }

  struct ASTErrorList* errors = ctx->errors;
  if(errors->size > 0) {
    for(size_t i = 0; i < errors->size; i++) {
      printASTError(filename, &errors->errors[i]);
    }

    exit(1);
  }

  free(errors);
  free(ctx);
  return stmts;
}
