/* stmtlist.c */

#include "../util/reallocarray.h"
#include "stmtlist.h"
#include "statementAST.h"

struct StmtList newStmtList(void) {
  struct StmtList list;
  list.capacity = 4;
  list.size = 0;
  list.stmts = calloc(list.capacity, sizeof(struct StmtAST*));

  return list;
}

void appendToStmtList(struct StmtList* list, struct StmtAST* stmt) {
  if(list->size >= list->capacity) {
    list->capacity *= 2;
    list->stmts =
      reallocarray(list->stmts, list->capacity, sizeof(struct StmtAST*));
  }

  list->stmts[list->size++] = stmt;
}

void freeStmtList(struct StmtList* list) {
  for(size_t i = 0; i < list->size; i++)
    freeStmtNode(list->stmts[i]);

  free(list);
}
