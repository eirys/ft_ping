#include <string.h>
#include <getopt.h>

#include "options.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

Arguments g_arguments;

/* -------------------------------------------------------------------------- */

enum e_Option {
    OPTION_A = 0U,
    OPTION_B,
    OPTION_C,
    OPTION_HELP,

    OPTION_COUNT
};

enum e_ShortOptionFlag {
    FLAG_A = 'a',
    FLAG_B = 'b',
    FLAG_C = 'c',

    FLAG_UNKNOWN = '?',
    FLAG_END = -1,
};

static
const char* option_names[OPTION_COUNT] = {
    "a",
    "b",
    "c",
    "help",
};

/* --------------------------------- STATIC --------------------------------- */

static
void _enable_flag(i32* flag) {
    if (flag == NULL) {
        log_debug("_enable_flag", "flag is NULL");
        return;
    }

    *flag = 1;
}

static
FT_RESULT _set_flag(char** flag) {
    if (flag == NULL) {
        log_debug("_set_flag", "improper address for flag");
        return FT_FAILURE;
    } else if (optarg == NULL) {
        log_error("bad value for `%s'", option_names[optind]);
        return FT_FAILURE;
    } else if (*flag != NULL) {
        log_error("flag `%s' already set", option_names[optind]);
        return FT_FAILURE;
    }

    *flag = optarg;

    return FT_SUCCESS;
}

static
FT_RESULT _retrieve_options(const i32 arg_count, char* const* arg_values) {
    memset(&g_arguments, 0, sizeof(Arguments));

    Options* options = &g_arguments.m_options;

    struct option option_descriptors[OPTION_COUNT + 1]= {
        { option_names[OPTION_A],       no_argument,        0,                  FLAG_A },
        { option_names[OPTION_B],       no_argument,        0,                  FLAG_B },
        { option_names[OPTION_C],       required_argument,  0,                  FLAG_C },
        { option_names[OPTION_HELP],    no_argument,        &options->m_help,   1 },
        { 0, 0, 0, 0 }
    };
    const char* short_options = "abc:";

    while (true) {
        i32 option_result = getopt_long(arg_count, arg_values, short_options, option_descriptors, NULL);

        switch (option_result) {
            // No option argument
            case FLAG_A:            _enable_flag(&options->m_flag_a); break;
            case FLAG_B:            _enable_flag(&options->m_flag_b); break;

            // Option argument
            case FLAG_C:            if (_set_flag(&options->m_flag_c) == FT_FAILURE) { return FT_FAILURE; } break;

            case FLAG_UNKNOWN:      return FT_FAILURE;

            case FLAG_END:
            default:                return FT_SUCCESS;
        }
    }

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

FT_RESULT retrieve_arguments(const i32 arg_count, char* const* arg_values) {
    if (_retrieve_options(arg_count, arg_values) == FT_FAILURE) {
        return FT_FAILURE;
    }

    g_arguments.m_destination = arg_values[optind];

    if (optind + 1 < arg_count) {
        log_error("Too many arguments");
        return FT_FAILURE;
    } else if (g_arguments.m_destination == NULL) {
        log_error("No destination provided");
        return FT_FAILURE;
    }

    log_info("destination: `%s'", g_arguments.m_destination);

    return FT_SUCCESS;
}