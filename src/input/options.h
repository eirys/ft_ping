#ifndef OPTIONS_H
# define OPTIONS_H

#include "typedefs.h"

typedef struct s_Options {
    i32     m_flag_a;
    i32     m_flag_b;
    char*   m_flag_c;
    i32     m_help;
} Options;

typedef struct s_Arguments {
    Options m_options;
    char*  m_destination;
} Arguments;

extern Arguments g_arguments;

FT_RESULT retrieve_options(const int arg_count, char* const* arg_values);

#endif // OPTIONS_H