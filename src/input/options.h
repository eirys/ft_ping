#ifndef OPTIONS_H
# define OPTIONS_H

#include "typedefs.h"

/* -------------------------------------------------------------------------- */
/*                                   STRUCTS                                  */
/* -------------------------------------------------------------------------- */

typedef struct s_Pattern {
    u64     content;    /* hex content */
    u32     length;     /* Pattern length in bytes (0 - 16) */
} Pattern;

/**
 * @brief Holds program flags.
 */
typedef struct s_Options {
    Pattern m_pattern;
    i32     m_linger;
    u8      m_ttl;
    bool    m_verbose;
    bool    m_help;
    bool    m_numeric;
} Options;

/**
 * @brief Handles program arguments.
 */
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