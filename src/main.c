/* main.c */

#include <stdio.h>
#include <errno.h>

#include <sys/stat.h>

#include "parser.h"

void panic(int err, const char* msg) {
  perror(msg);
  exit(err);
}

size_t getFileSize(const char* filename) {
  struct stat* stbuf = malloc(sizeof(*stbuf));
  int err = stat(filename, stbuf);

  if(err) panic(-1, "Failed to get filesize");

  size_t size = stbuf->st_size;
  free(stbuf);

  return size;
}

const char* readFile(const char* filename) {
  FILE* file = fopen(filename, "r");
  if(!file) panic(1, "Failed to open file");

  size_t filesize = getFileSize(filename);
  char* program = calloc(filesize, sizeof(char));

  fread(program, filesize, sizeof(char), file);

  return program;
}

int main(void) {
  const char* program = readFile("./build/test.uc");
  struct TokenArray* tokens = tokenise(program);

  for(size_t i = 0; i < tokens->capacity; i++) {
    printf("%ld: %20s, %10s, (%ld, %ld)\n", i,
        getTokenName(tokens->tokens[i].type),
        tokens->tokens[i].literal,
        tokens->tokens[i].row,
        tokens->tokens[i].col);
  }

  freeTokenArray(tokens);
  return 0;
}
