/* main.c */

#include "parser/parser.h"

int main(void) {
  struct StmtList* stmts = parseProgram("./build/test.uc");

  freeStmtList(stmts);

  return 0;
}
