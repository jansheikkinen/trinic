/* textcolor.c */

#include <stdio.h>
#include "textcolor.h"

void setColor(int attr, int fg, int bg) {
  printf("%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
}

void resetColor(void) {
  setColor(COLATTR_RESET, COL_DEFAULT, COL_DEFAULT);
}
