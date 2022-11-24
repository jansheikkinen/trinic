/* argumentAST.c */

// For use in variable assignments and function calls. i.e.
// let x: int8, y: mut float32 = 69, 42.0.
// The left side is an ArgLV, and the right is an ArgRV.

#include <stdio.h>

#include "../util/printIndent.h"
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

static void printIdentifierArg(const struct IdentifierArg* arg, size_t indent) {
  PRINT_INDENT(indent); printf("Identifier_Arg %s\n", arg->identifier);
  if(arg->type)
    printTypeAST(arg->type, indent + 1);
}

static void printAssignArg(const struct AssignArg* arg, size_t indent) {
  PRINT_INDENT(indent); printf("Assign_Arg %s\n", arg->identifier);
  if(arg->rval) printExprAST(arg->rval, indent + 1);
}

static void printSumArgType(const struct SumArgType* arg, size_t indent) {
  PRINT_INDENT(indent); printf("Sum_Arg_Type ");
  if(arg->type == SUMARG_IDENTIFIER)
    printf("%s\n", arg->as.identifier);
  else if(arg->type == SUMARG_TYPE) {
    printf("\n");
    printTypeAST(arg->as.type, indent + 1);
  }
}

static void printSumArg(const struct SumArg* arg, size_t indent) {
  PRINT_INDENT(indent); printf("Sum_Arg ");
  printf("%s\n", arg->name);
  if(arg->fields)
    printArgAST(arg->fields, indent + 1);
}

static void printGenDef(const struct GenericDef* arg, size_t indent) {
  PRINT_INDENT(indent); printf("Generic_Def\n");
  if(arg->lvalue) printArgAST(arg->lvalue, indent + 1);
  if(arg->rvalue) printArgAST(arg->rvalue, indent + 1);
}

static void printMatchArm(const struct MatchArm* arg, size_t indent) {
  PRINT_INDENT(indent); printf("Match_Arm\n");
  printArgAST(arg->exprs, indent + 1);
  printStmtList(arg->body, indent + 1);
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

void printArgAST(const struct ArgAST* args, size_t indent) {
  PRINT_INDENT(indent); printf("Arg ");

  switch(args->type) {
    case ARG_UNDEFINED: printf("UNDEFINED\n"); break;
    case ARG_EXPR:
      printf("EXPR\n");
      for(size_t i = 0; i < args->as.exprargs.args->size; i++)
        printExprAST(args->as.exprargs.args->members[i], indent + 1);
      break;
    case ARG_IDENTIFIER:
      printf("IDENTIFIER\n");
      for(size_t i = 0; i < args->as.idargs.size; i++)
        printIdentifierArg(args->as.idargs.members[i], indent + 1);
      break;
    case ARG_ASSIGN:
      printf("ASSIGN\n");
      for(size_t i = 0; i < args->as.assignargs.size; i++)
        printAssignArg(args->as.assignargs.members[i], indent + 1);
      break;
    case ARG_SUMARG:
      printf("SUMARG\n");
      for(size_t i = 0; i < args->as.sums.size; i++)
        printSumArg(args->as.sums.members[i], indent + 1);
      break;
    case ARG_SUMARGTYPE:
      printf("SUMARGTYPE\n");
      for(size_t i = 0; i < args->as.sumtypes.size; i++)
        printSumArgType(args->as.sumtypes.members[i], indent + 1);
      break;
    case ARG_GENERIC:
      printf("GENERIC\n");
      for(size_t i = 0; i < args->as.generics.size; i++) {
        PRINT_INDENT(indent + 1);
        printf("IDENTIFIER %s\n", args->as.generics.members[i]);
      }
      break;
    case ARG_GENERIC_DEFL:
      printf("GENERIC_DEFL\n");
      for(size_t i = 0; i < args->as.genleft.size; i++) {
        PRINT_INDENT(indent + 1);
        printf("IDENTIFIER %s\n", args->as.genleft.members[i]);
      }
      break;
    case ARG_GENERIC_DEFR:
      printf("GENERIC_DEFR\n");
      for(size_t i = 0; i < args->as.genright.size; i++)
        printTypeAST(args->as.genright.members[i], indent + 1);
      break;
    case ARG_GENERIC_DEFS:
      printf("GENERIC_DEFS\n");
      for(size_t i = 0; i < args->as.gendefs.size; i++)
        printGenDef(args->as.gendefs.members[i], indent + 1);
      break;
    case ARG_MATCH_ARMS:
      printf("MATCH_ARM\n");
      for(size_t i = 0; i < args->as.matcharms.size; i++)
        printMatchArm(args->as.matcharms.members[i], indent + 1);
      break;
  }
}
