#ifndef GENERATEAST_H
#define GENERATEAST_H

#include "token.h"
#include "stmtlist.h"

struct StmtList* generateAST(const char*, struct TokenArray*);

#endif
