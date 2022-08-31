/* tokeniser.c */

#include "tokeniser.h"

void newTokeniser(struct TokeniserData* td, const char* __restrict__ program) {
  td->tokens = initialiseTokenArray();
  td->program = program;
  td->index = 0;
  td->row = 1;
  td->col = 1;
  td->tokenStart = 0;
}

void nextToken(struct TokeniserData* td) {
  td->col += 1; td->index += 1; td->tokenStart = td->index;
}
