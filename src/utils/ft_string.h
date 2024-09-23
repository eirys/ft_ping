#ifndef STRING_H
# define STRING_H

#include "typedefs.h"

struct s_String {

    char*   m_data;
    u32     m_length;

}; // struct s_String

typedef struct s_String String;

String  createString(const char* data);
void    destroyString(String* string);

#endif // STRING_H