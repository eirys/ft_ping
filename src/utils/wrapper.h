#ifndef STRING_H
# define STRING_H

#include "typedefs.h"

void*       Malloc(const u32 size);
FT_RESULT   Free(void* data);

char*       Strdup(const char* data);

int         Socket(int domain, int type, int protocol);
FT_RESULT   Close(int fd);

#endif /* STRING_H */