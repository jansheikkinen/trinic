/* tokeniser.c */

#include "../util/reallocarray.h"
#include "tokeniser.h"

static void attachLexErrorList(struct LexerContext* ctx) {
  struct LexErrorList* errors = malloc(sizeof(*errors));

  errors->capacity = 4;
  errors->size = 0;

  errors->errors = calloc(errors->capacity, sizeof(*errors->errors));

  ctx->errors = errors;
}

void newTokeniser(struct LexerContext* td, const char* __restrict__ program) {
  attachLexErrorList(td);
  td->tokens = initialiseTokenArray();
  td->program = program;
  td->index = 0;
  td->row = 1;
  td->col = 1;
  td->tokenStart = 0;
}

void nextToken(struct LexerContext* td) {
  td->col += 1; td->index += 1; td->tokenStart = td->index;
}

void appendLexError(struct LexerContext* ctx, struct LexError error) {
  struct LexErrorList* errors = ctx->errors;

  if(errors->capacity >= errors->size) {
    errors->capacity *= 1.5;

    errors->errors =
      reallocarray(errors->errors, errors->capacity, sizeof(*errors->errors));
  }

  errors->errors[errors->size++] = error;
}
