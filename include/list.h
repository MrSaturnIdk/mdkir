#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct {
    char** list;
    char* allocated;
    intmax_t len;
} CommandList;

extern const char* COMMAND_LIST;

CommandList getList(void);
int listIncludes(const char* target);
int addToList(const char* item);
int removeFromList(const char* item);

#endif
