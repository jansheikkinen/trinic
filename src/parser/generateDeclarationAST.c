/* generateDeclarationAST.c */

// Everything that's allowed at the top level of the program, save for variable
// declarations, which I've implemented as a statement, since its the only
// declaration allowed at both levels

#include "generateStatementAST.h"
#include "generateTypeAST.h"
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
    struct ArgAST* generics = NULL; \
    if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) { \
      ctx->index += 1; \
      generics = generateGenericArguments(ctx); \
      if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) { \
        ctx->index += 1; \
      } else { \
        APPEND_ASTERROR(ctx, ASTERR_UNCLOSED_PAREN); \
        return NULL; \
      } \
    } \
    struct ArgAST* fields = argfnname(ctx); \
    if(MATCH_TOKEN(ctx, TOKEN_END)) { \
      ctx->index += 1; \
      return allocfnname(name, fields, generics); \
    } \
  } else { \
    APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER); \
  } \
  return NULL; \
}

DEFINE_GEN(genStruct, generateIdentifierArguments, allocNewStruct)
DEFINE_GEN(genUnion,  generateIdentifierArguments, allocNewUnion)
DEFINE_GEN(genSum,    generateSumArguments, allocNewSum)

static struct DeclarationAST* genEnum(struct ASTContext* ctx) {
  ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* name = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    struct ArgAST* fields = generateAssignmentArguments(ctx);
    if(MATCH_TOKEN(ctx, TOKEN_END)) {
      ctx->index += 1;
      return allocNewEnum(name, fields, NULL);
    }
  } else APPEND_ASTERROR(ctx, ASTERR_EXPECTED_IDENTIFIER);
  return NULL;
}

static struct DeclarationAST* genFunctionHeader(struct ASTContext* ctx) {
  ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* name = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
      ctx->index += 1;

      struct ArgAST* args = NULL;
      if(MATCH_TOKEN(ctx, TOKEN_VOID)) ctx->index += 1;
      else args = generateIdentifierArguments(ctx);

      if(MATCH_TOKEN(ctx, TOKEN_RIGHT_PAREN)) {
        ctx->index += 1;

        if(MATCH_TOKEN(ctx, TOKEN_ARROW)) {
          ctx->index += 1;

          struct TypeAST* returns = generateType(ctx);
          struct ArgAST* contracts = NULL, *generics = NULL;

          if(MATCH_TOKEN(ctx, TOKEN_WHERE)) {
            ctx->index += 1;
            contracts = generateExpressionArguments(ctx);
          }
          if(MATCH_TOKEN(ctx, TOKEN_FOR)) {
            ctx->index += 1;
            generics = generateGenericDefs(ctx);
          }

          return
            allocNewFunction(name, args, returns, contracts, generics, NULL);
        }
      }
    }
  }
  return NULL;
}

static struct DeclarationAST* genInterface(struct ASTContext* ctx) {
  ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* name = GET_CURRENT_TOKEN(ctx).literal;
    ctx->index += 1;

    struct DeclarationList* functions = malloc(sizeof(*functions));
    NEW_ARRAYLIST(functions);

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_FUNCTION)) {
        struct DeclarationAST* header = genFunctionHeader(ctx);
        APPEND_ARRAYLIST(functions, header);
      } else {
        if(MATCH_TOKEN(ctx, TOKEN_END)) {
          ctx->index += 1;
          return allocNewInterface(name, functions);
        }
      }
    }
  }
  return NULL;
}

static struct DeclarationAST* genFunction(struct ASTContext* ctx) {
  struct DeclarationAST* ast = genFunctionHeader(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_DO)) {
    ctx->index += 1;
    struct StmtList* body = malloc(sizeof(*body));
    *body = newStmtList();

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_END)) {
        ctx->index += 1;
        ast->as.function.body = body;
        return ast;
      } else appendToStmtList(body, generateStatement(ctx));
    }
  }

  return ast;
}

struct DeclarationAST* generateDeclaration(struct ASTContext* ctx) {
  if(MATCH_TOKEN(ctx, TOKEN_STRUCT)) return genStruct(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_UNION)) return genUnion(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_ENUM)) return genEnum(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_SUM)) return genSum(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_INTERFACE)) return genInterface(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_FUNCTION)) return genFunction(ctx);
  else if(MATCH_TOKEN(ctx, TOKEN_LET))
    return allocNewVarDeclDecl(generateStatement(ctx));
  else return NULL;
}
