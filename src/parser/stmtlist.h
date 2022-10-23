#ifndef STMTLIST_H
#define STMTLIST_H

#include <stdlib.h>

struct StmtList {
  struct StmtAST** stmts;
  size_t size;
  size_t capacity;
};

struct StmtList newStmtList(void);
void appendToStmtList(struct StmtList*, struct StmtAST*);
void freeStmtList(struct StmtList*);

#endif
