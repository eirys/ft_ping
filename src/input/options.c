#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <stdlib.h>

#include "options.h"
#include "log.h"
#include "wrapper.h"

/* --------------------------------- GLOBALS -------------------------------- */

Arguments g_arguments;

/* -------------------------------------------------------------------------- */

/**
 * getopt_long() global values
 *
 * @param optind    : (int, set to 1) Index of next element in processing queue for getopt.
 * @param optarg    : (char*) In case of option with argument, contains argument string value.
 *
 * @param opterr    : (int, set to 1) Enables automatic error message. Set to 0 to disable.
 * @param optopt    : (int) In case of error, contains errorneous character option.
 */

static int option; /* Current option being processed */

/* -------------------------------------------------------------------------- */

enum e_OptionIndex {
    OPT_INDEX_T = 0U,
    OPT_INDEX_V,
    OPT_INDEX_P,
    OPT_INDEX_HELP,
    OPT_INDEX_HELP_QM,

    OPTION_COUNT
};

enum e_ShortOptionFlag {
    FLAG_TTL = 't',
    FLAG_V = 'v',
    FLAG_P = 'p',
    FLAG_HELP_QM = '?',

    FLAG_END = -1,
};

/* ---------------------------------- TOOLS --------------------------------- */

static
void _enable_flag(i32* flag) {
    if (flag == NULL) {
        log_debug("_enable_flag", "flag is NULL");
        return;
    }

    *flag = 1;
}

static
FT_RESULT _set_flag(void* flag, FT_RESULT (*process_value)(void*, void*)) {
    if (flag == NULL) {
        log_debug("_set_flag", "improper address for flag");
        return FT_FAILURE;

    } else if (optarg == NULL) { /* optarg detailed above */
        log_error("bad value for `%c'", option);
        return FT_FAILURE;

    } else if (process_value(optarg, flag) == FT_FAILURE) {
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

/* -------------------------------- CALLBACKS ------------------------------- */

/**
 * @brief Check if the value is a u8 number, then set flag to integer conversion.
 */
static
FT_RESULT _ttl_check(void* value, void* pflag) {
    const char* copy = value;

    while (*copy != '\0') {
        if (!isdigit(*copy)) {
            log_error("invalid argument: `%s`", (char*)value);
            return FT_FAILURE;
        }
        ++copy;
    }

    const int result = atoi(value);
    if (result < 0 || result > UINT8_MAX) {
        log_error("failed to set ttl flag (value is outside of [0 - 255])");
        return FT_FAILURE;
    }

    *(u8*)pflag = (u8)result;

    return FT_SUCCESS;
}

static
FT_RESULT _hex_check(void* value, void* pflag) {
    const char* copy = value;
    u32         len = 0;

    while (*copy) {
        if (!isxdigit(*copy)) {
            log_error("invalid argument: `%s`", (char*)value);
            return FT_FAILURE;
        }
        ++copy;
        ++len;
    }

    if (len > 16) {
        log_error("failed to set pattern flag (value too long, only up to 16 bytes allowed)");
        return FT_FAILURE;
    }

    *(char**)pflag = Strdup(value);
    if (*(u8**)pflag == NULL) {
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

static inline
void _init_options(Options* options) {
    options->m_ttl = UINT8_MAX;
    options->m_pattern = NULL;
    options->m_verbose = 0;
    options->m_help = 0;
}

static
FT_RESULT _retrieve_options(const int arg_count, char* const* arg_values) {
    memset(&g_arguments, 0, sizeof(Arguments));

    Options* options = &g_arguments.m_options;
    _init_options(options);

    struct option option_descriptors[OPTION_COUNT + 1] = {
        /* Short options */
        { "?",      no_argument,        0,                  FLAG_HELP_QM },
        { "t",      required_argument,  0,                  FLAG_TTL },
        { "v",      no_argument,        0,                  FLAG_V },
        { "p",      required_argument,  0,                  FLAG_P },
        /* Long options */
        { "help",   no_argument,        &options->m_help,   1 },
        { 0, 0, 0, 0 }
    };

    const char* short_options = "?t:vp:";

    while (true) {
        option = getopt_long(arg_count, arg_values, short_options, option_descriptors, NULL);

        switch (option) {
            /* No argument */
            case FLAG_V:            _enable_flag(&options->m_verbose); break;
            case FLAG_HELP_QM:      if (optopt == 0) { _enable_flag(&options->m_help); break; } else { return FT_FAILURE; }

            /* With argument */
            case FLAG_TTL:          if (_set_flag((void*)&options->m_ttl, _ttl_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_P:            if (_set_flag((void*)&options->m_pattern, _hex_check) == FT_FAILURE) { return FT_FAILURE; } break;

            case FLAG_END:          return FT_SUCCESS;
            default:                return FT_FAILURE;
        }


    }

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

FT_RESULT retrieve_arguments(const int arg_count, char* const* arg_values) {
    if (_retrieve_options(arg_count, arg_values) == FT_FAILURE) {
        return FT_FAILURE;
    }

    if (g_arguments.m_options.m_help == 1) {
        return FT_SUCCESS;
    }

    g_arguments.m_destination = arg_values[optind];

    if (optind + 1 < arg_count) {
        log_error("too many arguments (use flag `--help` for indications)");
        return FT_FAILURE;
    } else if (g_arguments.m_destination == NULL) {
        log_error("no destination provided (use flag `--help` for indications)");
        return FT_FAILURE;
    }

    log_info("destination: `%s'", g_arguments.m_destination);

    return FT_SUCCESS;
}

void destroy_options(void) {
    Options* options = &g_arguments.m_options;

    if (options->m_pattern != NULL)
        Free(options->m_pattern);
}