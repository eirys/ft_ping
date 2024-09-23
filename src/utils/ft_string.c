#include "ft_string.h"

#include <string.h>
#include <stdlib.h>

String createString(const char* data) {
    if (data == NULL) {
        return (String) { NULL, 0 };
    }

    String string;
    string.m_data = strdup(data);
    string.m_length = strlen(data);

    return string;
}

void destroyString(String* string) {
    if (string->m_data != NULL) {
        free(string->m_data);
        string->m_data = NULL;
    }
    string->m_length = 0;
}