/* panic.c */

#include <stdio.h>
#include <stdlib.h>

void panic(int err, const char* msg) {
  perror(msg);
  exit(err);
}
