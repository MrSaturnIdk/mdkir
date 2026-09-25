/// Little string helper

#include "utils/string.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>

int hasspace(const char* str) {
    assert(str);
    for (size_t i = 0; str[i]; ++i) {
        if (isspace((unsigned char)str[i])) {
            return 1;
        }
    }
    return 0;
}
