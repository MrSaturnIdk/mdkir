#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct {
    char** list;
    intmax_t len;
} CommandList;

CommandList getList(void);
int listIncludes(const char* target);
int addToList(char* item);
int removeFromList(const char* item);

#endif
