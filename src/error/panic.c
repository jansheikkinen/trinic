/* panic.c */

#include <stdio.h>
#include <stdlib.h>

void panic(int err, const char* msg) {
  printf("ERROR: %s\n", msg);
  exit(err);
}
