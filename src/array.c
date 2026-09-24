/**
 * One helper for arrays
 * I don't wanna clog my main file
 */

#include "array.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

int findIndexOfString(const char* restrict array[], size_t len, const char* restrict item) {
    assert(item);
    for (size_t i = 0; i < len; ++i) {
        assert(array[i]);
        if (!strcmp(array[i], item)) {
            return (int)i;
        }
    }
    return -1;
}
