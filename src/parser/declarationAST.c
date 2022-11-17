/* declarationAST.c */

#include <stdio.h>

#include "declarationAST.h"

#define ALLOC_NEW_STRUCTURE(structuren, enum) \
  struct DeclarationAST* structuren(const char* name, \
      struct ArgAST* fields) { \
    struct DeclarationAST* ast = malloc(sizeof(*ast)); \
    ast->type = enum; \
    ast->name = name; \
    ast->as.structure.fields = fields; \
    return ast; \
  }

ALLOC_NEW_STRUCTURE(allocNewStruct, DECLARATION_STRUCT)
ALLOC_NEW_STRUCTURE(allocNewUnion, DECLARATION_UNION)
ALLOC_NEW_STRUCTURE(allocNewEnum, DECLARATION_ENUM)
ALLOC_NEW_STRUCTURE(allocNewSum, DECLARATION_SUM)
ALLOC_NEW_STRUCTURE(allocNewInterface, DECLARATION_INTERFACE)

struct DeclarationAST* allocNewFunction(const char* name, struct ArgAST* args,
    struct TypeAST* returns, struct ArgAST* contracts,
    struct ArgAST* generics) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->type = DECLARATION_FUNCTION;
  ast->name = name;

  ast->as.function.args = args;
  ast->as.function.returns = returns;
  ast->as.function.contracts = contracts;
  ast->as.function.generics = generics;

  return ast;
}

void freeDeclarationAST(struct DeclarationAST* ast) {
  switch(ast->type) {
    case DECLARATION_UNDEFINED: break;
    case DECLARATION_STRUCT:
    case DECLARATION_UNION:
    case DECLARATION_ENUM:
    case DECLARATION_SUM:
    case DECLARATION_INTERFACE:
      freeArgAST(ast->as.structure.fields); break;
    case DECLARATION_FUNCTION:
      freeArgAST(ast->as.function.args);
      freeArgAST(ast->as.function.contracts);
      freeArgAST(ast->as.function.generics);
      freeTypeAST(ast->as.function.returns); break;
  }

  free(ast);
}

void printDeclarationAST(const struct DeclarationAST* ast) {
  switch(ast->type) {
    case DECLARATION_UNDEFINED: printf("DECL_UNDEFINED"); break;
    case DECLARATION_STRUCT:
      printf("struct %s ", ast->name);
      printArgAST(ast->as.structure.fields);
      printf(" end"); break;
    case DECLARATION_UNION:
      printf("union %s ", ast->name);
      printArgAST(ast->as.structure.fields);
      printf(" end"); break;
    case DECLARATION_ENUM:
      printf("enum %s ", ast->name);
      printArgAST(ast->as.structure.fields);
      printf(" end"); break;
    case DECLARATION_SUM:
      printf("sum %s ", ast->name);
      printArgAST(ast->as.structure.fields);
      printf(" end"); break;
    case DECLARATION_INTERFACE:
      printf("interface %s ", ast->name);
      printArgAST(ast->as.structure.fields);
      printf(" end"); break;
    case DECLARATION_FUNCTION:
      printf("function %s(", ast->name);

      if(ast->as.function.args) printArgAST(ast->as.function.args);
      else printf("void");

      printf(" -> ");
      if(ast->as.function.returns) printTypeAST(ast->as.function.returns);

      if(ast->as.function.contracts) {
        printf(" where ");
        printArgAST(ast->as.function.contracts);
      }
      if(ast->as.function.generics) {
        printf(" for ");
        printArgAST(ast->as.function.generics);
      }
  }
}
