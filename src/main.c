/* main.c */

#include <stdio.h>
#include <errno.h>

#include "parser/token.h"
#include "parser/parser.h"
#include "parser/ast.h"

void panic(int err, const char* msg) {
  perror(msg);
  exit(err);
}

size_t getFileSize(FILE* file) {
  fseek(file, 0, SEEK_END);
  size_t filesize = ftell(file);
  fseek(file, 0, SEEK_SET);
  return filesize;
}

const char* readFile(const char* filename) {
  FILE* file = fopen(filename, "r");
  if(!file) panic(1, "Failed to open file");

  size_t filesize = getFileSize(file);
  char* program = calloc(filesize, sizeof(char));

  fread(program, filesize, sizeof(char), file);

  return program;
}

int main(void) {
  const char* program = readFile("./build/test.uc");
  struct TokenArray* tokens = tokenise(program);

  printf("### TOKENISER ###\n");
  for(size_t i = 0; i < tokens->capacity; i++) {
    printf("%03ld: %20s, %10s, (%03ld, %03ld)\n", i,
        getTokenName(tokens->tokens[i].type),
        tokens->tokens[i].literal,
        tokens->tokens[i].row,
        tokens->tokens[i].col);
  }


  printf("\n### AST GENERATION ###\n");
  struct AST* ast = generateAST(tokens);

  printASTNode(ast);

  freeASTNode(ast);

  return 0;
}
