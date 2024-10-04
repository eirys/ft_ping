#include "wrapper.h"
#include "log.h"

#include <string.h>
#include <stdlib.h>

/* ========================================================================== */
/*                                 ALLOCATION                                 */
/* ========================================================================== */

void* Malloc(const u32 size) {
    void* data = malloc(size);
    if (data == NULL) {
        log_error("Malloc", "failed to allocate memory");
        return NULL;
    }
    return data;
}

FT_RESULT Free(void* data) {
    if (data == NULL) {
        log_error("Free", "data is NULL");
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
        log_error("Strlen", "data is NULL");
        return 0;
    }
    return (u32)strlen(data);
}

char* Strdup(const char* data) {
    if (data == NULL) {
        log_error("Strdup", "data is NULL");
        return NULL;
    }
    char* str = strdup(data);
    if (str == NULL) {
        log_error("Strdup", "failed to duplicate string");
        return NULL;
    }
    return str;
}