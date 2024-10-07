#include "wrapper.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static
void _error(const char* function_name, const char* message) {
    fprintf(stderr, "[Error: `%s'] %s\n", function_name, message);
}

/* ========================================================================== */
/*                                 ALLOCATION                                 */
/* ========================================================================== */

void* Malloc(const u32 size) {
    void* data = malloc(size);
    if (data == NULL) {
        _error("Malloc", "failed to allocate memory");
        return NULL;
    }
    return data;
}

FT_RESULT Free(void* data) {
    if (data == NULL) {
        _error("Free", "data is NULL");
        return FT_FAILURE;
    }
    free(data);
    return FT_SUCCESS;
}

/* ========================================================================== */
/*                                   STRING                                   */
/* ========================================================================== */

u32 Strlen(const char* data) {
    if (data == NULL) {
        _error("Strlen", "data is NULL");
        return 0;
    }
    return (u32)strlen(data);
}

char* Strdup(const char* data) {
    if (data == NULL) {
        _error("Strdup", "data is NULL");
        return NULL;
    }
    char* str = strdup(data);
    if (str == NULL) {
        _error("Strdup", "failed to duplicate string");
        return NULL;
    }
    return str;
}