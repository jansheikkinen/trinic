/* declarationAST.c */

#include <stdio.h>

#include "declarationAST.h"

#define ALLOC_NEW_STRUCTURE(structuren, enum) \
  struct DeclarationAST* structuren(const char* name, \
      struct ArgAST* fields, struct ArgAST* generics) { \
    struct DeclarationAST* ast = malloc(sizeof(*ast)); \
    ast->type = enum; \
    ast->name = name; \
    ast->as.structure.fields = fields; \
    ast->as.structure.generics = generics; \
    return ast; \
  }

ALLOC_NEW_STRUCTURE(allocNewStruct, DECLARATION_STRUCT)
ALLOC_NEW_STRUCTURE(allocNewUnion, DECLARATION_UNION)
ALLOC_NEW_STRUCTURE(allocNewEnum, DECLARATION_ENUM) // too lazy to fix but enum
ALLOC_NEW_STRUCTURE(allocNewSum, DECLARATION_SUM) // should never have generics

struct DeclarationAST* allocNewInterface(const char* name,
    struct DeclarationList* functions) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->type = DECLARATION_INTERFACE;
  ast->name = name;
  ast->as.interface = functions;

  return ast;
}

struct DeclarationAST* allocNewFunction(const char* name, struct ArgAST* args,
    struct TypeAST* returns, struct ArgAST* contracts,
    struct ArgAST* generics, struct StmtList* body) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->type = DECLARATION_FUNCTION;
  ast->name = name;

  ast->as.function.args = args;
  ast->as.function.returns = returns;
  ast->as.function.contracts = contracts;
  ast->as.function.generics = generics;
  ast->as.function.body = body;

  return ast;
}

struct DeclarationAST* allocNewVarDeclDecl(struct StmtAST* stmt) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->type = DECLARATION_VARIABLE;
  ast->as.vardecl = stmt;

  return ast;
}

void freeDeclarationAST(struct DeclarationAST* ast) {
  switch(ast->type) {
    case DECLARATION_UNDEFINED: break;
    case DECLARATION_STRUCT:
    case DECLARATION_UNION:
    case DECLARATION_ENUM:
    case DECLARATION_SUM:
      if(ast->as.structure.fields) freeArgAST(ast->as.structure.fields);
      if(ast->as.structure.generics) freeArgAST(ast->as.structure.generics);
      break;
    case DECLARATION_INTERFACE:
      FREE_SELF_AND_MEMBERS(ast->as.interface, freeDeclarationAST); break;
    case DECLARATION_FUNCTION:
      if(ast->as.function.args)
        freeArgAST(ast->as.function.args);
      if(ast->as.function.contracts)
        freeArgAST(ast->as.function.contracts);
      if(ast->as.function.generics)
        freeArgAST(ast->as.function.generics);
      if(ast->as.function.returns)
        freeTypeAST(ast->as.function.returns);
      if(ast->as.function.body)
        freeStmtList(ast->as.function.body);
      break;
    case DECLARATION_VARIABLE:
      freeStmtNode(ast->as.vardecl); break;
  }

  free(ast);
}

void printDeclarationAST(const struct DeclarationAST* ast) {
  switch(ast->type) {
    case DECLARATION_UNDEFINED: printf("DECL_UNDEFINED"); break;
    case DECLARATION_STRUCT:
      printf("struct %s ", ast->name);
      if(ast->as.structure.generics) {
        printf("\b(");
        printArgAST(ast->as.structure.generics);
        printf(") ");
      }
      if(ast->as.structure.fields) printArgAST(ast->as.structure.fields);
      printf(" end\n\n"); break;
    case DECLARATION_UNION:
      printf("union %s ", ast->name);
      if(ast->as.structure.generics) {
        printf("\b(");
        printArgAST(ast->as.structure.generics);
        printf(") ");
      }
      if(ast->as.structure.fields) printArgAST(ast->as.structure.fields);
      printf(" end\n\n"); break;
    case DECLARATION_ENUM:
      printf("enum %s ", ast->name);
      if(ast->as.structure.fields) printArgAST(ast->as.structure.fields);
      printf(" end\n\n"); break;
    case DECLARATION_SUM:
      printf("sum %s ", ast->name);
      if(ast->as.structure.generics) {
        printf("\b(");
        printArgAST(ast->as.structure.generics);
        printf(") ");
      }
      if(ast->as.structure.fields) printArgAST(ast->as.structure.fields);
      printf(" end\n\n"); break;
    case DECLARATION_INTERFACE:
      printf("trait %s\n  ", ast->name);
      for(size_t i = 0; i < ast->as.interface->size; i++) {
        printDeclarationAST(ast->as.interface->members[i]);
        printf("\n  ");
      }
      printf("\b\bend\n\n"); break;
    case DECLARATION_FUNCTION:
      printf("function %s(", ast->name);

      if(ast->as.function.args) printArgAST(ast->as.function.args);
      else printf("void");

      printf(") -> ");
      if(ast->as.function.returns) printTypeAST(ast->as.function.returns);

      if(ast->as.function.contracts) {
        printf(" where ");
        printArgAST(ast->as.function.contracts);
      }

      if(ast->as.function.generics) {
        printf(" for ");
        printArgAST(ast->as.function.generics);
      }
      if(ast->as.function.body) {
        printf(" do\n  ");
        printStmtList(ast->as.function.body);
        printf("\b\bend\n\n");
      } break;
    case DECLARATION_VARIABLE:
      printStmtAST(ast->as.vardecl); printf("\n\n"); break;
  }
}
