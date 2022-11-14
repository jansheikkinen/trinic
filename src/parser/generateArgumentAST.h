#ifndef GENERATE_ARGUMENT_AST_H
#define GENERATE_ARGUMENT_AST_H

#include "ASTContext.h"

struct ArgAST* generateIdentifierArguments(struct ASTContext*);
struct ArgAST* generateExpressionArguments(struct ASTContext*);

#endif
