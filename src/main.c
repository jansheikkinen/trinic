/* main.c */

#include "error/panic.h"
#include "parser/parser.h"

int main(int argc, char** argv) {
  if(argc != 2) panic(1, "Please provide a file to compile");

  struct StmtList* stmts = parseProgram(argv[1]);

  freeStmtList(stmts);

  return 0;
}
