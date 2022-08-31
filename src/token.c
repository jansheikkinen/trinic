/* token.c */

#include "reallocarray.h"
#include "token.h"

struct Token newToken(enum TokenType type, char* literal,
    size_t row, size_t col) {
  struct Token token;

  token.type = type;
  token.literal = literal;
  token.row = row;
  token.col = col;

  return token;
}

void freeToken(struct Token* token) {
  free(token->literal);
  // free(token);
}


struct TokenArray* initialiseTokenArray(void) {
  struct TokenArray* tokenArray = malloc(sizeof(*tokenArray));

  tokenArray->capacity = 4;
  tokenArray->length   = 0;
  struct Token* tokens = calloc(tokenArray->capacity, sizeof(*tokens));
  tokenArray->tokens   = tokens;

  return tokenArray;
}

void appendToTokenArray(struct TokenArray* array, struct Token token) {
  if(array->length >= array->capacity) {
    array->capacity *= 2;
    array->tokens =
      reallocarray(array->tokens, array->capacity, sizeof(struct Token));
  }
  array->tokens[array->length++] = token;
}

void freeTokenArray(struct TokenArray* array) {
  for(size_t i = 0; i < array->length; i++) {
    freeToken(array->tokens + i);
  }
  free(array);
}
