#ifndef TOKENISER_H
#define TOKENISER_H

#include <stdlib.h>

#include "token.h"

struct TokeniserData {
  struct TokenArray* tokens;
  const char* program;
  size_t index;
  size_t row;
  size_t col;
  size_t tokenStart;
};

void newTokeniser(struct TokeniserData*, const char* __restrict__);
void nextToken(struct TokeniserData*);

#define get(td)  td->program[td->index]
#define peek(td) td->program[td->index + 1]
#define over(td) td->program[td->index + 2]

#endif
