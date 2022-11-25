/* declarationAST.c */

#include <stdio.h>

#include "../util/printIndent.h"
#include "declarationAST.h"

#define ALLOC_NEW_STRUCTURE(structuren, enum) \
  struct DeclarationAST* structuren(const char* name, \
      struct ArgAST* fields, struct ArgAST* generics, bool isexternal) { \
    struct DeclarationAST* ast = malloc(sizeof(*ast)); \
    ast->type = enum; \
    ast->isexternal = isexternal; \
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
    struct DeclarationList* functions, bool isexternal) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->type = DECLARATION_INTERFACE;
  ast->name = name;
  ast->isexternal = isexternal;
  ast->as.interface = functions;

  return ast;
}

struct DeclarationAST* allocNewFunction(const char* name, struct ArgAST* args,
    struct TypeAST* returns, struct ArgAST* contracts,
    struct ArgAST* generics, struct StmtList* body, bool isexternal) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->type = DECLARATION_FUNCTION;
  ast->name = name;
  ast->isexternal = isexternal;

  ast->as.function.args = args;
  ast->as.function.returns = returns;
  ast->as.function.contracts = contracts;
  ast->as.function.generics = generics;
  ast->as.function.body = body;

  return ast;
}

struct DeclarationAST* allocNewVarDeclDecl(struct StmtAST* stmt,
    bool isexternal) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->isexternal = isexternal;
  ast->type = DECLARATION_VARIABLE;
  ast->as.vardecl = stmt;

  return ast;
}

struct DeclarationAST* allocNewImpl(struct TypeAST* trait, struct TypeAST* type,
    struct DeclarationList* body, bool isexternal) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->isexternal = isexternal;
  ast->type = DECLARATION_IMPL;
  ast->as.impl.trait = trait;
  ast->as.impl.type = type;
  ast->as.impl.body = body;

  return ast;
}

struct DeclarationAST* allocNewInclude(const char* file) {
  struct DeclarationAST* ast = malloc(sizeof(*ast));

  ast->isexternal = false;
  ast->type = DECLARATION_INCLUDE;
  ast->name = file;

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
    case DECLARATION_IMPL:
      freeTypeAST(ast->as.impl.trait);
      freeTypeAST(ast->as.impl.type);
      FREE_SELF_AND_MEMBERS(ast->as.impl.body, freeDeclarationAST); break;
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
    case DECLARATION_INCLUDE: break;
  }

  free(ast);
}

void printDeclarationAST(const struct DeclarationAST* ast, size_t indent) {
  PRINT_INDENT(indent); printf("Declaration ");

  switch(ast->type) {
    case DECLARATION_UNDEFINED: printf("UNDEFINED"); break;
    case DECLARATION_STRUCT:
      printf("STRUCT\n");
      if(ast->as.structure.generics)
        printArgAST(ast->as.structure.generics, indent + 1);
      if(ast->as.structure.fields)
        printArgAST(ast->as.structure.fields, indent + 1);
      break;
    case DECLARATION_UNION:
      printf("UNION\n");
      if(ast->as.structure.generics)
        printArgAST(ast->as.structure.generics, indent + 1);
      if(ast->as.structure.fields)
        printArgAST(ast->as.structure.fields, indent + 1);
      break;
    case DECLARATION_ENUM:
      printf("ENUM\n");
      if(ast->as.structure.fields)
        printArgAST(ast->as.structure.fields, indent + 1);
      break;
    case DECLARATION_SUM:
      printf("SUM\n");
      if(ast->as.structure.generics)
        printArgAST(ast->as.structure.generics, indent + 1);
      if(ast->as.structure.fields)
        printArgAST(ast->as.structure.fields, indent + 1);
      break;
    case DECLARATION_INTERFACE:
      printf("TRAIT %s\n", ast->name);
      for(size_t i = 0; i < ast->as.interface->size; i++)
        printDeclarationAST(ast->as.interface->members[i], indent + 1);
      break;
    case DECLARATION_IMPL:
      printf("IMPL\n");
      printTypeAST(ast->as.impl.trait, indent + 1);
      printTypeAST(ast->as.impl.type, indent + 1);
      for(size_t i = 0; i < ast->as.impl.body->size; i++)
        printDeclarationAST(ast->as.impl.body->members[i], indent + 1);
      break;
    case DECLARATION_FUNCTION:
      printf("FUNCTION ");
      if(ast->name) printf(" %s", ast->name);
      printf("\n");

      if(ast->as.function.args)
        printArgAST(ast->as.function.args, indent + 1);

      if(ast->as.function.returns)
        printTypeAST(ast->as.function.returns, indent + 1);

      if(ast->as.function.contracts)
        printArgAST(ast->as.function.contracts, indent + 1);

      if(ast->as.function.generics)
        printArgAST(ast->as.function.generics, indent + 1);

      if(ast->as.function.body)
        printStmtList(ast->as.function.body, indent + 1);
      break;
    case DECLARATION_VARIABLE:
      printf("\n");
      printStmtAST(ast->as.vardecl, indent + 1);
      break;
    case DECLARATION_INCLUDE:
      printf("INCLUDE %s\n", ast->name); break;
  }
  if(ast->isexternal) { PRINT_INDENT(indent + 1); printf("EXTERNAL\n"); }
}
