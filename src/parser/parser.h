#ifndef PARSER_H
#define PARSER_H

#include "declarationAST.h"

struct DeclarationList* parseProgram(const char* filename);

#endif
