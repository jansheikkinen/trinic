#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdlib.h>

#define DEFINE_ARRAYLIST(name, memtype) \
  struct name { memtype* members; size_t size; size_t capacity; }

#define APPEND_ARRAYLIST(list, member) \
  do { \
    if((list)->size >= (list)->capacity) { \
      (list)->capacity *= 2; \
      (list)->members = \
      realloc((list)->members, (list)->capacity * sizeof(*(list)->members)); \
    } \
    (list)->members[(list)->size++] = member; \
  } while(0)

// the calloc call gives me a bugprone-sizeof error; why?
#define NEW_ARRAYLIST(list) \
  do { \
    (list)->capacity = 4; \
    (list)->size = 0; \
    (list)->members = calloc((list)->capacity, sizeof(*(list)->members)); \
  } while(0)

#define FREE_MEMBERS(list, freefn) \
  for(size_t i = 0; i < (list)->size; i++) { freefn((list)->members[i]); }

#define PRINT_MEMBERS(list, printfn) \
  for(size_t i = 0; i < (list)->size; i++) { printfn((list)->members[i]); }

#define FREE_SELF_AND_MEMBERS(list, freefn) \
  do { FREE_MEMBERS(list, freefn); free(list); } while(0)

#endif
