/* expressionAST.c */

/* Related to functions needed to create expression AST nodes */

#include <stdio.h>

#include "../util/printIndent.h"
#include "argumentAST.h"
#include "declarationAST.h"
#include "expressionAST.h"


static struct ExprAST newLiteral(enum LiteralType type,
    union LiteralValue value) {
  struct ExprAST expr;

  struct LiteralExpression literal;
  literal.type = type;
  literal.as = value;

  expr.type = EXPR_LITERAL;
  expr.as.literal = literal;
  return expr;
}

static struct ExprAST newUnary(enum TokenType operation, struct ExprAST* operand) {
  struct ExprAST expr;

  struct UnaryExpression unary;
  unary.operation = operation;
  unary.operand = operand;

  expr.type = EXPR_UNARY;
  expr.as.unary = unary;
  return expr;
}

static struct ExprAST newBinary(enum TokenType operation,
  struct ExprAST* left, struct ExprAST* right) {
  struct ExprAST expr;

  struct BinaryExpression binary;
  binary.operation = operation;
  binary.left = left;
  binary.right = right;

  expr.type = EXPR_BINARY;
  expr.as.binary = binary;
  return expr;
}

static struct ExprAST newVariable(const char* identifier) {
  struct ExprAST expr;

  struct VariableExpression variable;
  variable.identifier = identifier;

  expr.type = EXPR_VARIABLE;
  expr.as.variable = variable;

  return expr;
}

static struct ExprAST newCall(struct ExprAST* callee, struct ArgAST* args) {
  struct ExprAST expr;

  struct CallExpression call;
  call.callee = callee;
  call.args = args;

  expr.type = EXPR_CALL;
  expr.as.call = call;

  return expr;
}

static struct ExprAST newGrouping(struct ExprAST* expression) {
  struct ExprAST expr;

  struct GroupingExpression grouping;
  grouping.expression = expression;

  expr.type = EXPR_GROUPING;
  expr.as.grouping = grouping;

  return expr;
}

static struct ExprAST newArrayInit(struct ArgAST* args) {
  struct ExprAST expr;

  struct ArrayInitExpression arr;
  arr.args = args;

  expr.type = EXPR_ARRAY_INIT;
  expr.as.arrinit = arr;

  return expr;
}

static struct ExprAST newCast(struct ExprAST* expr, struct TypeAST* type) {
  struct ExprAST ast;

  struct CastExpression cast;
  cast.expr = expr;
  cast.type = type;

  ast.type = EXPR_CAST;
  ast.as.cast = cast;

  return ast;
}

static struct ExprAST newUndefined(void) {
  struct ExprAST expr;

  expr.type = EXPR_UNDEFINED;

  return expr;
}

struct ExprAST* allocNewLiteral(enum LiteralType type,
    union LiteralValue value) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newLiteral(type, value);

  return ptr;
}

struct ExprAST* allocNewUnary(enum TokenType operation,
    struct ExprAST* operand) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newUnary(operation, operand);

  return ptr;
}

struct ExprAST* allocNewBinary(enum TokenType operation,
    struct ExprAST* left, struct ExprAST* right) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newBinary(operation, left,right);

  return ptr;
}

struct ExprAST* allocNewVariable(const char* identifier) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newVariable(identifier);

  return ptr;
}

struct ExprAST* allocNewCall(struct ExprAST* callee, struct ArgAST* args) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newCall(callee, args);

  return ptr;
}

struct ExprAST* allocNewGet(bool isPointer, const char* name,
    struct ExprAST* expr) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));

  ptr->type = EXPR_GET;
  ptr->as.get.isPointer = isPointer;
  ptr->as.get.name = name;
  ptr->as.get.expression = expr;

  return ptr;
}

struct ExprAST* allocNewGrouping(struct ExprAST* expr) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newGrouping(expr);

  return ptr;
}

struct ExprAST* allocNewArrayInit(struct ArgAST* args) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newArrayInit(args);

  return ptr;
}

struct ExprAST* allocNewUndefined(void) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newUndefined();

  return ptr;
}

struct ExprAST* allocNewArrayIndex(struct ExprAST* index,
    struct ExprAST* identifier) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));

  ptr->type = EXPR_ARRAY_INDEX;
  ptr->as.arrindex.index = index;
  ptr->as.arrindex.identifier = identifier;

  return ptr;
}

struct ExprAST* allocNewFunctionExpr(struct DeclarationAST* function) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));

  ptr->type = EXPR_FUNCTION;
  ptr->as.function.function = function;

  return ptr;
}

struct ExprAST* allocNewCastExpr(struct ExprAST* expr, struct TypeAST* type) {
  struct ExprAST* ptr = malloc(sizeof(*ptr));
  if(ptr) *ptr = newCast(expr, type);

  ptr->type = EXPR_CAST;

  return ptr;
}

void printExprAST(const struct ExprAST* ast, size_t indent) {
  PRINT_INDENT(indent); printf("Expr ");

  switch(ast->type) {
    case EXPR_UNDEFINED:
      printf("UNDEFINED\n"); break;
    case EXPR_LITERAL:
      printf("LITERAL\n"); PRINT_INDENT(indent + 1);
      switch(ast->as.literal.type) {
        case LIT_UNDEFINED:
          printf("UNDEFINED"); break;
        case LIT_IDENTIFIER:
          printf("IDENTIFIER %s", ast->as.literal.as.identifier); break;
        case LIT_STRING:
          printf("STRING %s", ast->as.literal.as.string); break;
        case LIT_CHAR:
          printf("CHAR %c", ast->as.literal.as.character); break;
        case LIT_INTEGER:
          printf("INT %ld", ast->as.literal.as.integer); break;
        case LIT_UINTEGER:
          printf("UINT %lu", ast->as.literal.as.uinteger); break;
        case LIT_FLOAT:
          printf("FLOAT %lf", ast->as.literal.as.floating); break;
        case LIT_BOOL:
          printf("BOOL %s", ast->as.literal.as.boolean?"true":"false"); break;
      } printf("\n"); break;
    case EXPR_UNARY:
      printf("UNARY %s\n", getTokenName(ast->as.unary.operation));
      printExprAST(ast->as.unary.operand, indent + 1);
      break;
    case EXPR_BINARY:
      printf("BINARY %s\n", getTokenName(ast->as.binary.operation));
      printExprAST(ast->as.binary.left, indent + 1);
      printExprAST(ast->as.binary.right, indent + 1);
      break;
    case EXPR_VARIABLE:
      printf("VARIABLE %s\n", ast->as.variable.identifier); break;
    case EXPR_GROUPING:
      printf("GROUPING\n");
      printExprAST(ast->as.grouping.expression, indent + 1);
      break;
    case EXPR_CALL:
      printf("CALL\n");
      printExprAST(ast->as.call.callee, indent + 1);
      if(ast->as.call.args) printArgAST(ast->as.call.args, indent + 1);
      break;
    case EXPR_GET:
      printf("GET");
      if(ast->as.get.isPointer) printf(" PTR");
      printf(" %s\n", ast->as.get.name);
      printExprAST(ast->as.get.expression, indent + 1); break;
    case EXPR_ARRAY_INDEX:
      printf("ARRAY INDEX\n");
      printExprAST(ast->as.arrindex.identifier, indent + 1);
      printExprAST(ast->as.arrindex.index, indent + 1);
      break;
    case EXPR_ARRAY_INIT:
      printf("ARRAY INIT\n");
      printArgAST(ast->as.arrinit.args, indent + 1);
      break;
    case EXPR_FUNCTION:
      printf("FUNCTION\n");
      printDeclarationAST(ast->as.function.function, indent + 1);
      break;
    case EXPR_CAST:
      printf("CAST\n");
      printExprAST(ast->as.cast.expr, indent + 1);
      printTypeAST(ast->as.cast.type, indent + 1);
      break;
  }
}

void freeExprNode(struct ExprAST* expr) {
  switch(expr->type) {
    case EXPR_UNDEFINED: break;
    case EXPR_LITERAL:
      switch(expr->as.literal.type) {
        case LIT_IDENTIFIER:
          free(expr->as.literal.as.identifier); break;
        case LIT_STRING:
          free(expr->as.literal.as.string); break;
        default: break;
      } break;
    case EXPR_UNARY:
      freeExprNode(expr->as.unary.operand); break;
    case EXPR_BINARY:
      freeExprNode(expr->as.binary.left);
      freeExprNode(expr->as.binary.right);
      break;
    case EXPR_VARIABLE:
      break;
    case EXPR_GROUPING:
      freeExprNode(expr->as.grouping.expression); break;
    case EXPR_CALL:
      freeExprNode(expr->as.call.callee);
      if(expr->as.call.args) freeArgAST(expr->as.call.args);
      break;
    case EXPR_GET:
      freeExprNode(expr->as.get.expression); break;
    case EXPR_ARRAY_INDEX:
      freeExprNode(expr->as.arrindex.index);
      freeExprNode(expr->as.arrindex.identifier); break;
    case EXPR_ARRAY_INIT:
      freeArgAST(expr->as.arrinit.args); break;
    case EXPR_FUNCTION:
      if(expr->as.function.function)
        freeDeclarationAST(expr->as.function.function);
      break;
    case EXPR_CAST:
      freeExprNode(expr->as.cast.expr);
      freeTypeAST(expr->as.cast.type);
      break;
  }

  free(expr);
}
