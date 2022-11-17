/* argumentAST.c */

// For use in variable assignments and function calls. i.e.
// let x: int8, y: mut float32 = 69, 42.0.
// The left side is an ArgLV, and the right is an ArgRV.

#include <stdio.h>

#include "argumentAST.h"
#include "expressionAST.h"

struct ArgAST* allocNewExprArgList(struct ExprList* args) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_EXPR;
  argast->as.exprargs.args = args;

  return argast;
}

struct IdentifierArg* allocNewIdentifierArg(const char* identifier,
    struct TypeAST* type) {
  struct IdentifierArg* arg = malloc(sizeof(*arg));

  arg->identifier = identifier;
  arg->type = type;

  return arg;
}

struct AssignArg* allocNewAssignArg(const char* identifier, long value) {
  struct AssignArg* arg = malloc(sizeof(*arg));

  arg->identifier = identifier;
  arg->value = value;

  return arg;
}

struct ArgAST* allocNewIdentifierArgList(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_IDENTIFIER;
  NEW_ARRAYLIST(&argast->as.idargs);

  return argast;
}

struct ArgAST* allocNewAssignArgList(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_ASSIGN;
  NEW_ARRAYLIST(&argast->as.assignargs);

  return argast;
}

void freeIdentifierArg(struct IdentifierArg* arg) {
  freeTypeAST(arg->type);
  free(arg);
}

void freeAssignArg(struct AssignArg* arg) {
  free(arg);
}

void printIdentifierArg(const struct IdentifierArg* arg) {
  if(arg->type) {
    printf("%s: ", arg->identifier);
    printTypeAST(arg->type);
  } else {
    printf("%s", arg->identifier);
  }
}

void printAssignArg(const struct AssignArg* arg) {
  printf("(= %s ", arg->identifier);
  if(arg->value) printf("%ld)", arg->value);
  else printf("- )");
}

void freeArgAST(struct ArgAST* args) {
  switch(args->type) {
    case ARG_UNDEFINED: break;
    case ARG_EXPR:
      FREE_SELF_AND_MEMBERS(args->as.exprargs.args, freeExprNode); break;
    case ARG_IDENTIFIER:
      FREE_MEMBERS(&args->as.idargs, freeIdentifierArg); break;
    case ARG_ASSIGN:
      FREE_MEMBERS(&args->as.assignargs, freeAssignArg); break;
  }
  free(args);
}

void printArgAST(const struct ArgAST* args) {
  switch(args->type) {
    case ARG_UNDEFINED: printf("ARG_UNDEFINED"); break;
    case ARG_EXPR:
      for(size_t i = 0; i < args->as.exprargs.args->size; i++) {
        printExprAST(args->as.exprargs.args->members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
    case ARG_IDENTIFIER:
      for(size_t i = 0; i < args->as.idargs.size; i++) {
        printIdentifierArg(args->as.idargs.members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
    case ARG_ASSIGN:
      for(size_t i = 0; i < args->as.assignargs.size; i++) {
        printAssignArg(args->as.assignargs.members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
  }
}
