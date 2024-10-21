#include <string.h>
#include <getopt.h>

#include "options.h"
#include "callbacks.h"
#include "log.h"
#include "wrapper.h"

#define RANDOM_PATTERN      0xDEADBEEF

/* --------------------------------- GLOBALS -------------------------------- */

Arguments g_arguments = {
    .m_options.m_ttl = UINT8_MAX,
    .m_options.m_pattern = RANDOM_PATTERN,
    .m_options.m_verbose = false,
    .m_options.m_help = false,

    .m_destination = NULL
};

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
void _enable_flag(bool* flag) {
    if (flag == NULL) {
        log_debug("_enable_flag", "flag is NULL");
        return;
    }

    *flag = true;
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

/* -------------------------------------------------------------------------- */

static inline
void _init_options(Options* options) {
}

static
FT_RESULT _retrieve_options(const int arg_count, char* const* arg_values) {
    memset(&g_arguments, 0, sizeof(Arguments));

    Options* options = &g_arguments.m_options;
    _init_options(options);

    struct option option_descriptors[OPTION_COUNT + 1] = {
        /* Short options */
        { "?",      no_argument,        NULL,                   FLAG_HELP_QM },
        { "t",      required_argument,  NULL,                   FLAG_TTL },
        { "v",      no_argument,        NULL,                   FLAG_V },
        { "p",      required_argument,  NULL,                   FLAG_P },
        /* Long options */
        { "help",   no_argument,        (int*)&options->m_help, (int)true },
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
            case FLAG_TTL:          if (_set_flag((void*)&options->m_ttl, ttl_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_P:            if (_set_flag((void*)&options->m_pattern, hex_check) == FT_FAILURE) { return FT_FAILURE; } break;

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

}