#ifndef STRUCTURE_AST_H
#define STRUCTURE_AST_H

#include "argumentAST.h"

enum StructureTypes {
  STRUCTURE_UNDEFINED,
  STRUCTURE_STRUCT,
  STRUCTURE_UNION,
  STRUCTURE_ENUM,
};

struct StructureAST {
  enum StructureTypes type;
  const char* name;
  struct ArgAST* fields;
};

struct StructureAST* allocNewStructure(const char*, struct ArgAST*);
struct StructureAST* allocNewUnion(const char*, struct ArgAST*);
struct StructureAST* allocNewEnum(const char*, struct ArgAST*);

void freeStructureAST(struct StructureAST*);

void printStructureAST(const struct StructureAST*);

#endif
