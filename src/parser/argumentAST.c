/* argumentAST.c */

// For use in variable assignments and function calls. i.e.
// let x: int8, y: mut float32 = 69, 42.0.
// The left side is an ArgLV, and the right is an ArgRV.

#include <stdio.h>

#include "argumentAST.h"
#include "expressionAST.h"

struct ArgAST* allocNewExprArg(struct ExprList* args) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_EXPR;
  argast->as.exprargs.args = args;

  return argast;
}

struct ArgAST* allocNewIdentifierArg(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_IDENTIFIER;
  NEW_ARRAYLIST(&argast->as.idargs);

  return argast;
}

void freeIdentifierArg(struct IdentifierArg* arg) {
  // TODO: implement and free type info
  free(arg);
}

void printIdentifierArg(const struct IdentifierArg* arg) {
  printf("(%s)", arg->identifier);
}

void freeArgAST(struct ArgAST* args) {
  switch(args->type) {
    case ARG_UNDEFINED: break;
    case ARG_EXPR:
      FREE_SELF_AND_MEMBERS(args->as.exprargs.args, freeExprNode); break;
    case ARG_IDENTIFIER: break;
      FREE_MEMBERS(&args->as.idargs, freeIdentifierArg); break;
  }
  free(args);
}

void printArgAST(const struct ArgAST* args) {
  switch(args->type) {
    case ARG_UNDEFINED: printf("(ARG_UNDEFINED)"); break;
    case ARG_EXPR:
      printf("(expr-args (");
      PRINT_MEMBERS(args->as.exprargs.args, printExprAST);
      printf("))"); break;
    case ARG_IDENTIFIER:
      printf("(identifier-args (");
      PRINT_MEMBERS(&args->as.idargs, printIdentifierArg);
      printf("))"); break;
  }
}
