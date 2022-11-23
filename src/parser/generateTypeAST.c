/* generateTypeAST.c */

#include "generateExpressionAST.h"
#include "token.h"
#include "generateTypeAST.h"
#include "generateArgumentAST.h"

#define GET_CURRENT_TOKEN(ctx) ctx->tokens->tokens[ctx->index]
#define MATCH_TOKEN(ctx, token) GET_CURRENT_TOKEN(ctx).type == token

static struct TypeAST* generateStructType(struct ASTContext* ctx,
    enum StructTypes type, bool ismutable) {
  ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
      ctx->index += 1;
      struct ArgAST* generics = genSumTypes(ctx);

      if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
        ctx->index += 1;
        return allocNewStructType(identifier, type, generics, ismutable);
      } else APPEND_ASTERROR(ctx, ASTERR_UNCLOSED_PAREN);
    } else return allocNewStructType(identifier, type, NULL, ismutable);
  } else APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER);
  return NULL;
}

static struct TypeAST* generateBaseType(struct ASTContext* ctx,
    bool ismutable) {
  for(enum TokenType type = TOKEN_INT8; type <= TOKEN_ANY; type++) {
    if(MATCH_TOKEN(ctx, type)) {
      ctx->index += 1;
      return allocNewBaseType(type - TOKEN_FALSE, ismutable);
    }
  }

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* identifier = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;
    return allocNewBaseTypeStr(identifier, ismutable);
  }

  APPEND_ASTERROR(ctx, ASTERR_EXPECTED_TYPE);
  return NULL;
}

static struct TypeAST* generateVariadicType(struct ASTContext* ctx,
    bool ismutable) {
  ctx->index += 1;
  return allocNewVariadicType(generateType(ctx), ismutable);
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
  } else APPEND_ASTERROR(ctx, ASTERR_UNCLOSED_BRACKET);

  return NULL;
}

static struct TypeAST* generateFunctionType(struct ASTContext* ctx,
    bool ismutable) {
  ctx->index += 1;

  struct ArgAST* params = NULL;
  struct TypeAST* returns = NULL;
  if(!(MATCH_TOKEN(ctx, TOKEN_VOID))) {
    params = genSumTypes(ctx);
  } else ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
    ctx->index += 1;
    if(MATCH_TOKEN(ctx, TOKEN_ARROW)) {
      ctx->index += 1;

      if(!(MATCH_TOKEN(ctx, TOKEN_VOID))) {
        returns = generateType(ctx);
      } else ctx->index += 1;
    } else APPEND_ASTERROR(ctx, ASTERR_EXPECTED_ARROW_FUNCTION);
  } else APPEND_ASTERROR(ctx, ASTERR_UNCLOSED_PAREN);

  struct ArgAST* generics = NULL;
  if(MATCH_TOKEN(ctx, TOKEN_FOR)) {
    ctx->index += 1;
    generics = generateGenericDefs(ctx);
  }

  return allocNewFunctionType(params, returns, generics, ismutable);
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
  else if(MATCH_TOKEN(ctx, TOKEN_DOT_DOT))
    return generateVariadicType(ctx, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_STAR))
    return generatePointerType(ctx, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_LEFT_BRACKET))
    return generateArrayType(ctx, ismutable);
  else if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN))
    return generateFunctionType(ctx, ismutable);
  else return generateBaseType(ctx, ismutable);
}
