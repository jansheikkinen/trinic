#ifndef GENERATEAST_H
#define GENERATEAST_H

#include "token.h"
#include "declarationAST.h"

struct DeclarationList* generateAST(const char*, struct TokenArray*);

#endif
