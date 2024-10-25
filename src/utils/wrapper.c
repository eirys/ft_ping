#include "wrapper.h"

#include <string.h> /* strdup */
#include <stdlib.h> /* malloc, free */
#include <unistd.h> /* close */
#include <sys/socket.h> /* socket */
#include <stdio.h> /* fprintf */
#include <sys/time.h> /* gettimeofday */

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
/*                                 RAW MEMORY                                 */
/* -------------------------------------------------------------------------- */

void Memset64(void* dest, u64 value, u32 count) {
    u64* ptr = (u64*)dest;
    for (u32 i = 0; i < count; ++i)
        ptr[i] = value;
}

/* -------------------------------------------------------------------------- */
/*                                   SOCKET                                   */
/* -------------------------------------------------------------------------- */

#if 0
int Select(
    int nfds,
    fd_set* readfds,
    fd_set* writefds,
    fd_set* exceptfds,
    struct timeval* timeout
) {
    int fds = select(nfds, readfds, writefds, exceptfds, timeout);
    if (fds == -1) {
        _error("Select", "failed to listen on file descriptor");
        return -1;
    } else if (fds == 0) {
        _error("Select", "timeout reached");
    }
    return fds;
}
#endif

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

FT_RESULT Sendto(int sockfd, const void* buf, u32 len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen) {
    const ssize_t bytes = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
    if (bytes == -1) {
        _error("Send", "failed to send data");
        return FT_FAILURE;
    } else if (bytes != len) {
        _error("Send", "failed to send all data");
        return FT_FAILURE;
    }
    return FT_SUCCESS;
}

ssize_t Recvfrom(int sockfd, void* buf, u32 len, int flags, struct sockaddr* src_addr, socklen_t* addrlen) {
    const ssize_t bytes = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (bytes == -1) {
        _error("Recv", "failed to receive data");
    }
    return bytes;
}

/* -------------------------------------------------------------------------- */
/*                                    OTHER                                   */
/* -------------------------------------------------------------------------- */

FT_RESULT Gettimeofday(struct timeval* tv, void* tz) {
    if (gettimeofday(tv, tz) == -1) {
        _error("Gettimeofday", "failed to send data");
        return FT_FAILURE;
    }
    return FT_SUCCESS;

}