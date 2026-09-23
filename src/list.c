/**
 * Small helper for handling the list (source)
 *
 * -2 = Memory error
 * -1 = I/O fail
 * 0 = Success
 * 1 = Also success, for usage in an if statement, otherwise bad input
 */

#include "list.h" // Pulls in CommandList

#include "ansicolors.h"
#include "array.h"

#include <unistd.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char* PROGRAM_NAME;
extern int STDERR_TTY;
extern int verbose;

/// Internal
static const char* COMMAND_LIST = SYSCONFDIR "/list";
static int changeList(CommandList newList) {
    if (verbose) {
        printf("Opening list\n");
        fflush(stdout);
    }
    FILE* list = fopen(COMMAND_LIST, "w");
    if (!list) {
        fprintf(stderr,  "%s: %serror:%s %sfailed to open %s%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            COMMAND_LIST,
            STDERR_TTY ? ANSI_RESET : ""
        );
        return -1;
    }

    if (verbose) {
        printf("Writing to list\n");
        fflush(stdout);
    }

    char catList[4096] = "";
    for (intmax_t i = 0; i < newList.len; ++i) {
        size_t size = strlen(catList);
        strncat(catList, newList.list[i], sizeof(catList) - size - 1);
        size = strlen(catList);
        strncat(catList, "\n", sizeof(catList) - size - 1);
    }

    fprintf(list, "%s", catList);
    fclose(list);
    return 0;
}

/// Public
CommandList getList(void) {
    CommandList list = {.list = NULL, .len = 0};

    // Open
    if (verbose) {
        printf("Opening list\n");
        fflush(stdout);
    }
    FILE* listFile = fopen(COMMAND_LIST, "r");
    if (!listFile) {
        fprintf(stderr,  "%s: %serror:%s %sfailed to open %s%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            COMMAND_LIST,
            STDERR_TTY ? ANSI_RESET : ""
        );
        list.len = -1;
        return list;
    }
    if (verbose) {
        printf("Getting contents of list\n");
        fflush(stdout);
    }

    // Read
    char* items = calloc(4096, sizeof(char));
    if (!items) {
        fprintf(stderr,  "%s: %serror:%s %sfailed to allocate memory%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            STDERR_TTY ? ANSI_RESET : ""
        );
        fclose(listFile);
        list.len = -2;
        return list;
    }
    size_t totalRead = fread(items, 1, 4095, listFile);
    items[totalRead] = '\0';
    if (ferror(listFile) || totalRead == 4095 && !feof(listFile)) {
        fprintf(stderr,  "%s: %serror:%s %sfailed to read %s%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            COMMAND_LIST,
            STDERR_TTY ? ANSI_RESET : ""
        );
        fclose(listFile);
        list.len = -2;
        return list;
    }
    fclose(listFile);
    
    // Turn into array
    char** arrayList = calloc(4096, sizeof(char*));
    if (!arrayList) {
        fprintf(stderr,  "%s: %serror:%s %sfailed to allocate memory%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            STDERR_TTY ? ANSI_RESET : ""
        );
        list.len = -2;
        return list;
    }
    for (int i = 0; i < 4096; ++i) {
        arrayList[i] = "";
    }

    intmax_t i = 0;
    for (char* item = strtok(items, "\n"); item; item = strtok(NULL, "\n")) {
        arrayList[i++] = item;
    }
    list.list = arrayList;
    list.len = i;
    return list;
}

int listIncludes(const char* target) {
    CommandList list = getList();
    if (!list.list) {
        return list.len;
    }
    if (verbose) {
        printf("Checking if list contains '%s'\n", target);
        fflush(stdout);
    }
    if (findIndexOfString((const char**)list.list, 4096, target) != -1) {
        free(*list.list);
        free(list.list);
        return 1;
    }
    free(*list.list);
    free(list.list);
    return 0;
}
int addToList(char* item) {
    if (strchr(item, ' ') || strchr(item, '\t') || strchr(item, '\n')) {
        fprintf(stderr, "%s: %serror:%s %sitem '%s' contains whitespace%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            item,
            STDERR_TTY ? ANSI_RESET : ""
        );
        return 1;
    }

    int listIncludesItem = listIncludes(item);
    switch (listIncludesItem) {
        case -1: {
            return -1;
        }
        case -2: {
            return -2;
        }
        case 1: {
            fprintf(stderr, "%s: %serror:%s %slist already includes '%s'%s\n",
                PROGRAM_NAME,
                STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                STDERR_TTY ? ANSI_RESET : "",
                STDERR_TTY ? ANSI_BOLD : "",
                item,
                STDERR_TTY ? ANSI_RESET : ""
            );
            return 1;
        }
    }

    CommandList list = getList();
    if (!list.list) {
        return list.len;
    }
    list.list[list.len] = item;
    ++list.len;

    int changed = changeList(list);
    if (changed) {
        free(*list.list);
        free(list.list);
        return changed;
    }
    if (verbose) {
        printf("Item '%s' added successfully\n", item);
        fflush(stdout);
    }
    free(*list.list);
    free(list.list);
    return 0;
}
int removeFromList(const char* item) {
    int listIncludesItem = listIncludes(item);
    switch (listIncludesItem) {
        case -1: {
            return -1;
        }
        case -2: {
            return -2;
        }
        case 0: {
            fprintf(stderr, "%s: %serror:%s %slist does not include '%s'%s\n",
                PROGRAM_NAME,
                STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                STDERR_TTY ? ANSI_RESET : "",
                STDERR_TTY ? ANSI_BOLD : "",
                item,
                STDERR_TTY ? ANSI_RESET : ""
            );
            return 1;
        }
    }

    CommandList list = getList();
    if (!list.list) {
        return list.len;
    }
    list.list[findIndexOfString((const char**)list.list, list.len, item)] = "";

    int changed = changeList(list);
    if (changed) {
        free(*list.list);
        free(list.list);
        return changed;
    }
    if (verbose) {
        printf("Item '%s' removed successfully\n", item);
        fflush(stdout);
    }
    free(*list.list);
    free(list.list);
    return 0;
}
