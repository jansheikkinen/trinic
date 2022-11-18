/* generateTypeAST.c */

#include "generateExpressionAST.h"
#include "token.h"
#include "generateTypeAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) GET_CURRENT_TOKEN(ctx).type == token

static struct TypeAST* generateStructType(struct ASTContext* ctx,
    enum StructTypes type, bool ismutable) {
  ctx->index += 1;
  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;
    return allocNewStructType(identifier, type, ismutable);
  } else {
    APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER);
    return NULL;
  }
}

static struct TypeAST* generateBaseType(struct ASTContext* ctx,
    bool ismutable) {
  for(enum TokenType type = TOKEN_INT8; type <= TOKEN_BOOL; type++) {
    if(MATCH_TOKEN(ctx, type)) {
      ctx->index += 1;
      return allocNewBaseType(type - TOKEN_FALSE, ismutable);
    }
  }

  APPEND_ASTERROR(ctx, ASTERR_EXPECTED_TYPE);
  return NULL;
}

static struct TypeAST* generatePointerType(struct ASTContext* ctx,
    bool ismutable) {
  ctx->index += 1;

  return allocNewPointerType(generateType(ctx), ismutable);
}

static struct TypeAST* generateArrayType(struct ASTContext* ctx,
    bool ismutable) {
  ctx->index += 1;

  struct ExprAST* size = NULL;
  if(!(MATCH_TOKEN(ctx, TOKEN_RIGHT_BRACKET))) size = generateExpression(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_RIGHT_BRACKET)) {
    ctx->index += 1;

    return allocNewArrayType(generateType(ctx), size, ismutable);
  }

  return NULL;
}

struct TypeAST* generateType(struct ASTContext* ctx) {
  bool ismutable = false;
  if(MATCH_TOKEN(ctx, TOKEN_MUT)) {
    ctx->index += 1;
    ismutable = true;
  }

  if(MATCH_TOKEN(ctx, TOKEN_STRUCT))
    return generateStructType(ctx, STRUCT_STRUCT, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_UNION))
    return generateStructType(ctx, STRUCT_UNION, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_ENUM))
    return generateStructType(ctx, STRUCT_ENUM, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_SUM))
    return generateStructType(ctx, STRUCT_SUM, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_INTERFACE))
    return generateStructType(ctx, STRUCT_INTERFACE, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_STAR))
    return generatePointerType(ctx, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_LEFT_BRACKET))
    return generateArrayType(ctx, ismutable);
  else return generateBaseType(ctx, ismutable);
}
