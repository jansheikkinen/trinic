/* generateTypeAST.c */

#include "generateExpressionAST.h"
#include "token.h"
#include "generateTypeAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) GET_CURRENT_TOKEN(ctx).type == token

struct TypeAST* generateBaseType(struct ASTContext* ctx) {
  for(enum TokenType type = TOKEN_INT8; type <= TOKEN_BOOL; type++) {
    if(MATCH_TOKEN(ctx, type)) {
      ctx->index += 1;

      struct TypeAST* base = allocNewBaseType(type - TOKEN_FALSE);

      if(MATCH_TOKEN(ctx, TOKEN_STAR)) {
        ctx->index += 1;

        base->type = TYPE_PTR;
        return base;

      } else if(MATCH_TOKEN(ctx, TOKEN_LEFT_BRACKET)) {
        ctx->index += 1;

        struct ExprAST* size = generateExpression(ctx);

        if(MATCH_TOKEN(ctx, TOKEN_RIGHT_BRACKET)) {
          ctx->index += 1;
          return allocNewArrayType(base, size);
        } else APPEND_ASTERROR(ctx, ASTERR_MISSING_RIGHT_BRACKET);

      } else return base;
    }
  }

  APPEND_ASTERROR(ctx, ASTERR_EXPECTED_TYPE);
  return NULL;
}

struct TypeAST* generateStructType(struct ASTContext* ctx,
    enum StructTypes type) {
  ctx->index += 1;
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;
    return allocNewStructType(identifier, type);
  } else {
    APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER);
    return NULL;
  }
}

struct TypeAST* generateType(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_STRUCT))
    return generateStructType(ctx, STRUCT_STRUCT);
  else if(MATCH_TOKEN(ctx, TOKEN_UNION))
    return generateStructType(ctx, STRUCT_UNION);
  else if(MATCH_TOKEN(ctx, TOKEN_ENUM))
    return generateStructType(ctx, STRUCT_ENUM);
  else if(MATCH_TOKEN(ctx, TOKEN_SUM))
    return generateStructType(ctx, STRUCT_SUM);
  else if(MATCH_TOKEN(ctx, TOKEN_INTERFACE))
    return generateStructType(ctx, STRUCT_INTERFACE);
  else return generateBaseType(ctx);
}
