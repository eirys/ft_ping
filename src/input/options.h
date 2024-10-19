#ifndef OPTIONS_H
# define OPTIONS_H

#include "typedefs.h"

/* -------------------------------------------------------------------------- */
/*                                   STRUCTS                                  */
/* -------------------------------------------------------------------------- */

typedef struct s_Options {
    u8      m_ttl;          /* 0 - 255 */
    u8*     m_pattern;      /* hex content, up to 16 char */
    i32     m_verbose;
    i32     m_help;
} Options;

typedef struct s_Arguments {
    Options m_options;
    char*   m_destination;
} Arguments;

/* -------------------------------------------------------------------------- */
/*                                   GLOBALS                                  */
/* -------------------------------------------------------------------------- */

extern Arguments g_arguments;

/* -------------------------------------------------------------------------- */
/*                                 PROTOTYPES                                 */
/* -------------------------------------------------------------------------- */

FT_RESULT   retrieve_arguments(const int arg_count, char* const* arg_values);
void        destroy_options(void);

#endif /* OPTIONS_H */