#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

extern char* tokenTypeStrings[];
#define getTokenName(tokentype) tokenTypeStrings[tokentype]

struct TokenArray* tokenise(const char* __restrict__);

#endif
