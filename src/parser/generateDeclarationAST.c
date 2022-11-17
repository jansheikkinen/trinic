/* generateDeclarationAST.c */

// Everything that's allowed at the top level of the program, save for variable
// declarations, which I've implemented as a statement, since its the only
// declaration allowed at both levels

#include "generateDeclarationAST.h"

#include "generateArgumentAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) (GET_CURRENT_TOKEN(ctx).type == token)

#define DEFINE_GEN(fnname, argfnname, allocfnname) \
static struct DeclarationAST* fnname(struct ASTContext* ctx) { \
  ctx->index += 1; \
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) { \
    const char* name = GET_CURRENT_TOKEN(ctx).literal; \
    ctx->index += 1; \
    struct ArgAST* fields = argfnname(ctx); \
    if(MATCH_TOKEN(ctx, TOKEN_END)) return allocfnname(name, fields); \
  } else { \
    APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER); \
  } \
  return NULL; \
}

DEFINE_GEN(genStruct, generateIdentifierArguments, allocNewStruct)
DEFINE_GEN(genUnion,  generateIdentifierArguments, allocNewUnion)
DEFINE_GEN(genEnum,   generateAssignmentArguments, allocNewEnum)

struct DeclarationAST* generateDeclaration(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_STRUCT)) return genStruct(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_UNION)) return genUnion(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_ENUM)) return genEnum(ctx);
  else return NULL;
}
