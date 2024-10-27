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
    Pattern m_pattern;      /* -p --pattern */
    i32     m_linger;       /* -W --linger */
    u32     m_interval;     /* -i --interval */
    u32     m_count;        /* -c --count */
    u8      m_ttl;          /* --ttl */
    bool    m_verbose;      /* -v --verbose */
    bool    m_help;         /* -? --help */
    bool    m_numeric;      /* -n --numeric */
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

#endif /* OPTIONS_H */