#ifndef STRING_H
# define STRING_H

#include <sys/socket.h> /* socket */
#include <signal.h> /* sigaction */

#include "typedefs.h"

/* ------------------------------- ALLOCATION ------------------------------- */
void*       Malloc(const u32 size);
FT_RESULT   Free(void* data);

/* --------------------------------- STRING --------------------------------- */
char*       Strdup(const char* data);

/* ------------------------------- RAW MEMORY ------------------------------- */
void        Memset64(void* dest, u64 value, u32 size);

/* --------------------------------- SOCKET --------------------------------- */
int         Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);

int         Socket(int domain, int type, int protocol);
FT_RESULT   Close(int fd);

FT_RESULT   Sendto(int sockfd, const void* buf, u32 len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
ssize_t     Recvfrom(int sockfd, void* buf, u32 len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);

/* --------------------------------- SYSTEM --------------------------------- */
FT_RESULT   Gettimeofday(struct timeval* tv, void* tz);
FT_RESULT   Sigaction(int signum, const struct sigaction* new_act, struct sigaction* old_act);

#endif /* STRING_H */