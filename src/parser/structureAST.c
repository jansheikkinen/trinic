/* structureAST.c */

#include <stdio.h>
#include "structureAST.h"

#define ALLOC_NEW_STRUCTURE(structure, enum) \
  struct StructureAST* structure(const char* name, \
      struct ArgAST* fields) { \
    struct StructureAST* ast = malloc(sizeof(*ast)); \
    ast->type = enum; \
    ast->name = name; \
    ast->fields = fields; \
    return ast; \
  }

ALLOC_NEW_STRUCTURE(allocNewStructure, STRUCTURE_STRUCT)
ALLOC_NEW_STRUCTURE(allocNewUnion, STRUCTURE_UNION)
ALLOC_NEW_STRUCTURE(allocNewEnum, STRUCTURE_ENUM)

void freeStructureAST(struct StructureAST* ast) {
  switch(ast->type) {
    case STRUCTURE_UNDEFINED: break;
    case STRUCTURE_STRUCT:
    case STRUCTURE_UNION:
    case STRUCTURE_ENUM:
      freeArgAST(ast->fields);
  }

  free(ast);
}

void printStructureAST(const struct StructureAST* ast) {
  switch(ast->type) {
    case STRUCTURE_UNDEFINED: printf("UNDEFINED STRUCTURE"); break;
    case STRUCTURE_STRUCT:
      printf("(struct %s ", ast->name);
      printArgAST(ast->fields);
      printf(")"); break;
    case STRUCTURE_UNION:
      printf("(union %s ", ast->name);
      printArgAST(ast->fields);
      printf(")"); break;
    case STRUCTURE_ENUM:
      printf("(enum %s ", ast->name);
      printArgAST(ast->fields);
      printf(")"); break;
  }
}
