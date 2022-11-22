/* argumentAST.c */

// For use in variable assignments and function calls. i.e.
// let x: int8, y: mut float32 = 69, 42.0.
// The left side is an ArgLV, and the right is an ArgRV.

#include <stdio.h>

#include "argumentAST.h"
#include "expressionAST.h"
#include "statementAST.h"
#include "stmtlist.h"

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

struct AssignArg* allocNewAssignArg(const char* identifier,
    struct ExprAST* rval) {
  struct AssignArg* arg = malloc(sizeof(*arg));

  arg->identifier = identifier;
  arg->rval = rval;

  return arg;
}

struct SumArg* allocNewSumArg(const char* name, struct ArgAST* fields) {
  struct SumArg* arg = malloc(sizeof(*arg));

  arg->name = name;
  arg->fields = fields;

  return arg;
}

struct SumArgType* allocNewSumArgTypeType(struct TypeAST* field) {
  struct SumArgType* arg = malloc(sizeof(*arg));

  arg->type = SUMARG_TYPE;
  arg->as.type = field;

  return arg;
}

struct SumArgType* allocNewSumArgTypeStr(const char* name) {
  struct SumArgType* arg = malloc(sizeof(*arg));

  arg->type = SUMARG_IDENTIFIER;
  arg->as.identifier = name;

  return arg;
}

struct GenericDef* allocNewGenericDef(struct ArgAST* lv, struct ArgAST* rv) {
  struct GenericDef* arg = malloc(sizeof(*arg));

  arg->lvalue = lv;
  arg->rvalue = rv;

  return arg;
}

struct MatchArm* allocNewMatchArm(struct ArgAST* expr, struct StmtList* body) {
  struct MatchArm* arg = malloc(sizeof(*arg));

  arg->exprs = expr;
  arg->body = body;

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

struct ArgAST* allocNewGenericList(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_GENERIC;
  NEW_ARRAYLIST(&argast->as.generics);

  return argast;
}

struct ArgAST* allocNewSumArgList(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_SUMARG;
  NEW_ARRAYLIST(&argast->as.sums);

  return argast;
}

struct ArgAST* allocNewSumArgTypeList(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_SUMARGTYPE;
  NEW_ARRAYLIST(&argast->as.sumtypes);

  return argast;
}

struct ArgAST* allocNewGenericDefLeft(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_GENERIC_DEFL;
  NEW_ARRAYLIST(&argast->as.genleft);

  return argast;
}

struct ArgAST* allocNewGenericDefRight(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_GENERIC_DEFR;
  NEW_ARRAYLIST(&argast->as.genright);

  return argast;
}

struct ArgAST* allocNewGenericDefs(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_GENERIC_DEFS;
  NEW_ARRAYLIST(&argast->as.gendefs);

  return argast;
}

struct ArgAST* allocNewMatchArms(void) {
  struct ArgAST* argast = malloc(sizeof(*argast));

  argast->type = ARG_MATCH_ARMS;
  NEW_ARRAYLIST(&argast->as.matcharms);

  return argast;
}

static void freeIdentifierArg(struct IdentifierArg* arg) {
  if(arg->type) freeTypeAST(arg->type);
  free(arg);
}

static void freeAssignArg(struct AssignArg* arg) {
  if(arg->rval) freeExprNode(arg->rval);
  free(arg);
}

static void freeSumArgType(struct SumArgType* arg) {
  switch(arg->type) {
    case SUMARG_UNDEFINED:
    case SUMARG_IDENTIFIER: break;
    case SUMARG_TYPE: freeTypeAST(arg->as.type); break;
  }

  free(arg);
}

static void freeSumArg(struct SumArg* arg) {
  if(arg->fields) freeArgAST(arg->fields);
  free(arg);
}

static void freeGenDef(struct GenericDef* arg) {
  if(arg->lvalue) freeArgAST(arg->lvalue);
  if(arg->rvalue) freeArgAST(arg->rvalue);
  free(arg);
}

static void freeMatchArm(struct MatchArm* arg) {
  freeArgAST(arg->exprs);
  freeStmtList(arg->body);
}

static void printIdentifierArg(const struct IdentifierArg* arg) {
  if(arg->type) {
    printf("%s: ", arg->identifier);
    printTypeAST(arg->type);
  } else {
    printf("%s", arg->identifier);
  }
}

static void printAssignArg(const struct AssignArg* arg) {
  printf("%s", arg->identifier);
  if(arg->rval) {
    printf(" = ");
    printExprAST(arg->rval);
  }
}

static void printSumArgType(const struct SumArgType* arg) {
  if(arg->type == SUMARG_IDENTIFIER) printf("%s", arg->as.identifier);
  else if(arg->type == SUMARG_TYPE) printTypeAST(arg->as.type);
}

static void printSumArg(const struct SumArg* arg) {
  printf("%s", arg->name);
  if(arg->fields) {
    printf("(");
    printArgAST(arg->fields);
    printf(")");
  }
}

static void printGenDef(const struct GenericDef* arg) {
  if(arg->lvalue) printArgAST(arg->lvalue);
  printf(": ");
  if(arg->rvalue) printArgAST(arg->rvalue);
}

static void printMatchArm(const struct MatchArm* arg) {
  printArgAST(arg->exprs);
  printf(" => ");
  if(arg->body->size >= 2) {
    printf("do\n"); printStmtList(arg->body); printf(" end");
  } else for(size_t i = 0; i < arg->body->size; i++) {
    printStmtAST(arg->body->stmts[i]); printf(",\n  ");
  }
}

void freeArgAST(struct ArgAST* args) {
  switch(args->type) {
    case ARG_UNDEFINED:
    case ARG_GENERIC_DEFL:
    case ARG_GENERIC: break;
    case ARG_EXPR:
      FREE_SELF_AND_MEMBERS(args->as.exprargs.args, freeExprNode); break;
    case ARG_IDENTIFIER:
      FREE_MEMBERS(&args->as.idargs, freeIdentifierArg); break;
    case ARG_ASSIGN:
      FREE_MEMBERS(&args->as.assignargs, freeAssignArg); break;
    case ARG_SUMARG:
      FREE_MEMBERS(&args->as.sums, freeSumArg); break;
    case ARG_SUMARGTYPE:
      FREE_MEMBERS(&args->as.sumtypes, freeSumArgType); break;
    case ARG_GENERIC_DEFR:
      FREE_MEMBERS(&args->as.genright, freeTypeAST); break;
    case ARG_GENERIC_DEFS:
      FREE_MEMBERS(&args->as.gendefs, freeGenDef); break;
    case ARG_MATCH_ARMS:
      FREE_MEMBERS(&args->as.matcharms, freeMatchArm); break;
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
    case ARG_SUMARG:
      for(size_t i = 0; i < args->as.sums.size; i++) {
        printSumArg(args->as.sums.members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
    case ARG_SUMARGTYPE:
      for(size_t i = 0; i < args->as.sumtypes.size; i++) {
        printSumArgType(args->as.sumtypes.members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
    case ARG_GENERIC:
      for(size_t i = 0; i < args->as.generics.size; i++) {
        printf("%s, ", args->as.generics.members[i]);
      } printf("\b\b  \b\b"); break;
    case ARG_GENERIC_DEFL:
      for(size_t i = 0; i < args->as.genleft.size; i++) {
        printf("%s + ", args->as.genleft.members[i]);
      } printf("\b\b\b   \b\b\b"); break;
    case ARG_GENERIC_DEFR:
      for(size_t i = 0; i < args->as.genright.size; i++) {
        printTypeAST(args->as.genright.members[i]);
        printf(" + ");
      } printf("\b\b\b   \b\b\b"); break;
    case ARG_GENERIC_DEFS:
      for(size_t i = 0; i < args->as.gendefs.size; i++) {
        printGenDef(args->as.gendefs.members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
    case ARG_MATCH_ARMS:
      for(size_t i = 0; i < args->as.matcharms.size; i++) {
        printMatchArm(args->as.matcharms.members[i]);
        printf(", ");
      } printf("\b\b  \b\b"); break;
  }
}
