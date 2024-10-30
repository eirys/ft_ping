#ifndef OPTIONS_H
# define OPTIONS_H

#include "typedefs.h"

/* -------------------------------------------------------------------------- */
/*                                   STRUCTS                                  */
/* -------------------------------------------------------------------------- */

typedef struct s_Pattern {
    union {
        u64     _content;    /* hex content */
        u8      _raw[8];   /* bytes content */
    } _un;
#define content _un._content
#define raw   _un._raw
    u32     length;     /* Pattern length in bytes (0 - 16) */
} Pattern;

/**
 * @brief Holds program flags.
 */
typedef struct s_Options {
    Pattern m_pattern;      /* -p --pattern : pattern to fill ICMP payload */
    u32     m_timeout;      /* -w --timeout : time to wait for a response */
    u32     m_interval;     /* -i --interval : interval between each packet */
    u32     m_count;        /* -c --count: number of packets to send */
    u32     m_size;         /* -s --size : size of the ICMP payload */
    u32     m_linger;       /* -W --linger : time to linger */
    u8      m_ttl;          /* --ttl : time to live */
    bool    m_numeric;      /* -n --numeric : display numeric addresses */
    bool    m_verbose;      /* -v --verbose */
    bool    m_help;         /* -? --help */
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