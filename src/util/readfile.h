#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include <stdlib.h>

size_t getFileSize(FILE*);
char* readFile(const char*);

#endif
