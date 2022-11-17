#ifndef GENERATE_ARGUMENT_AST_H
#define GENERATE_ARGUMENT_AST_H

#include "ASTContext.h"
#include "expressionAST.h"

struct ArgAST* generateIdentifierArguments(struct ASTContext*);
struct ArgAST* generateExpressionArguments(struct ASTContext*);
struct ArgAST* generateExpressionArgumentsWithFirst(struct ASTContext*,
    struct ExprAST*);
struct ArgAST* generateAssignmentArguments(struct ASTContext*);
struct ArgAST* generateGenericArguments(struct ASTContext*);
struct ArgAST* generateSumArguments(struct ASTContext*);
// no generateSumArgTypes since it'd only be called by ^

#endif
