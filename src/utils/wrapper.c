#include "wrapper.h"

#include <string.h> /* strdup */
#include <stdlib.h> /* malloc, free */
#include <unistd.h> /* close */
#include <sys/socket.h> /* socket */
#include <stdio.h> /* fprintf */

#ifdef __DEBUG
# include <errno.h>
#endif

static
void _error(const char* function_name, const char* message) {
    fprintf(stderr, "[Error: `%s'] %s\n", function_name, message);
#ifdef __DEBUG
    fprintf(stderr, "(code %d: %s)\n", errno, strerror(errno));
#endif
}

/* -------------------------------------------------------------------------- */
/*                                 ALLOCATION                                 */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/*                                   STRING                                   */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/*                                   SOCKET                                   */
/* -------------------------------------------------------------------------- */

int Socket(int domain, int type, int protocol) {
    int fd = socket(domain, type, protocol);
    if (fd == -1)
        _error("Socket", "failed to create socket");
    return fd;
}

FT_RESULT Close(int fd) {
    if (close(fd) == -1) {
        _error("Close", "failed to close socket");
        return FT_FAILURE;
    }
    return FT_SUCCESS;
}