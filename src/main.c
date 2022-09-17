/* main.c */

#include "parser/parser.h"

int main(void) {
  struct AST* ast = parseProgram("./build/test.uc");

  freeASTNode(ast);

  return 0;
}
