#ifndef DEBUG_H
#define DEBUG_H

#define FLAG_DEBUG 1
#define FLAG_LEX   2
#define FLAG_AST   4
#define FLAG_BC    8

#define HAS_FLAG(x, y) ((x & y) == y)
#define GET_STAGE(x) \
  ((x & FLAG_LEX) | (x & FLAG_AST) | (x & FLAG_BC))

#endif
