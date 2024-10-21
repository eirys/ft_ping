#ifndef STRING_H
# define STRING_H

#include <sys/socket.h>

#include "typedefs.h"

/* ------------------------------- ALLOCATION ------------------------------- */
void*       Malloc(const u32 size);
FT_RESULT   Free(void* data);

/* --------------------------------- STRING --------------------------------- */
char*       Strdup(const char* data);

/* ------------------------------- RAW MEMORY ------------------------------- */
void        Memset64(void* dest, u64 value, u32 size);

/* --------------------------------- SOCKET --------------------------------- */
int         Socket(int domain, int type, int protocol);
FT_RESULT   Close(int fd);

FT_RESULT   Sendto(int sockfd, const void* buf, u32 len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);

#endif /* STRING_H */