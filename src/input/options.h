#ifndef OPTIONS_H
# define OPTIONS_H

#include "typedefs.h"

/* -------------------------------------------------------------------------- */
/*                                   STRUCTS                                  */
/* -------------------------------------------------------------------------- */

typedef struct s_Options {
    u64     m_pattern;      /* hex content */
    u8      m_ttl;          /* 0 - 255 */
    bool    m_verbose;
    bool    m_help;
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