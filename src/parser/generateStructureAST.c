/* generateStructureAST.c */

#include "generateArgumentAST.h"
#include "generateStructureAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) (GET_CURRENT_TOKEN(ctx).type == token)

#define DEFINE_GEN(fnname, argfnname, allocfnname) \
static struct StructureAST* fnname(struct ASTContext* ctx) { \
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

DEFINE_GEN(genStruct, generateIdentifierArguments, allocNewStructure)
DEFINE_GEN(genUnion,  generateIdentifierArguments, allocNewUnion)
DEFINE_GEN(genEnum,   generateAssignmentArguments, allocNewEnum)

struct StructureAST* generateStructure(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_STRUCT)) return genStruct(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_UNION)) return genUnion(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_ENUM)) return genEnum(ctx);
  else return NULL;
}
