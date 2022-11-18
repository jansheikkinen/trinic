/* main.c */

#include "error/panic.h"
#include "parser/parser.h"
#include "util/arraylist.h"

int main(int argc, char** argv) {
  if(argc != 2) panic(1, "Please provide a file to compile");

  struct DeclarationList* decls = parseProgram(argv[1]);

  FREE_SELF_AND_MEMBERS(decls, freeDeclarationAST);
  return 0;
}
