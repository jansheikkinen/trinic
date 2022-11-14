#ifndef EXPRLIST_H
#define EXPRLIST_H

#include "../util/arraylist.h"
#include "expressionAST.h"

DEFINE_ARRAYLIST(ExprList, struct ExprAST*);

#endif
