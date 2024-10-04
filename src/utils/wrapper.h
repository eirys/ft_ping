#ifndef STRING_H
# define STRING_H

#include "typedefs.h"

void*       Malloc(const u32 size);
FT_RESULT   Free(void* data);

u32         Strlen(const char* data);
char*       Strdup(const char* data);

#endif // STRING_H