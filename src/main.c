/* main.c */

#include <stdio.h>

#include "parser.h"

int main(void) {
  const char* str = "isize a = 69";
  struct TokenArray* tokens = tokenise(str);

  printf("\n");
  printf("%02ld / %02ld\n", tokens->length, tokens->capacity);
  for(size_t i = 0; i < tokens->capacity; i++) {
    printf("%02ld: %20s, %s, (%02ld, %02ld)\n", i,
        getTokenName(tokens->tokens[i].type),
        tokens->tokens[i].literal,
        tokens->tokens[i].row,
        tokens->tokens[i].col);
  }

  freeTokenArray(tokens);
  return 0;
}
