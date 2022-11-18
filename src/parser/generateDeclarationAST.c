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
DEFINE_GEN(genSum,    generateSumArguments, allocNewSum)

static struct DeclarationAST* genFunctionHeader(struct ASTContext* ctx) {
  ctx->index += 1;

  if(MATCH_TOKEN(ctx, TOKEN_IDENTIFIER_LITERAL)) {
    const char* name = GET_CURRENT_TOKEN(ctx).literal;

    if(MATCH_TOKEN(ctx, TOKEN_LEFT_PAREN)) {
      ctx->index += 1;

      if(!MATCH_TOKEN(ctx, TOKEN_VOID)) {
        struct ArgAST* args = generateIdentifierArguments(ctx);

        if(MATCH_TOKEN(ctx, TOKEN_ARROW)) {
          ctx->index += 1;

          // TODO: implement returning generics
          struct TypeAST* returns = generateType(ctx);
          struct ArgAST* contracts = NULL, *generics = NULL;

          if(MATCH_TOKEN(ctx, TOKEN_WHERE))
            contracts = generateExpressionArguments(ctx);
          if(MATCH_TOKEN(ctx, TOKEN_FOR))
            generics = generateGenericArguments(ctx);

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

    struct DeclarationList* functions = malloc(sizeof(*functions));
    NEW_ARRAYLIST(functions);

    while(ctx->index < ctx->tokens->length) {
      if(MATCH_TOKEN(ctx, TOKEN_FUNCTION)) {
        struct DeclarationAST* header = genFunctionHeader(ctx);
        APPEND_ARRAYLIST(functions, header);
      } else return allocNewInterface(name, functions);
    }
  }
  return NULL;
}

static struct DeclarationAST* genFunction(struct ASTContext* ctx) {
  struct DeclarationAST* ast = genFunctionHeader(ctx);

  if(MATCH_TOKEN(ctx, TOKEN_DO)) {
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
  else return NULL;
}
