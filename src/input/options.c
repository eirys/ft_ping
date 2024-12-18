#include <string.h>
#include <getopt.h>

#include "options.h"
#include "callbacks.h"
#include "log.h"
#include "network_io.h"
#include "wrapper.h"

/* --------------------------------- GLOBALS -------------------------------- */

Arguments g_arguments = {
    .m_options.m_pattern.pattern_content = 0xDEadBEef,
    .m_options.m_pattern.length = 4U,
    .m_options.m_timeout = UINT32_MAX,
    .m_options.m_interval = 1U,
    .m_options.m_count = UINT32_MAX,
    .m_options.m_size = ICMP_MSG_SIZE - ICMP_HEADER_SIZE, /* Default is 56 */
    .m_options.m_linger = UINT32_MAX,
    .m_options.m_ttl = UINT8_MAX,
    .m_options.m_verbose = false,
    .m_options.m_help = false,
    .m_options.m_numeric = false,
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

static int _current_option; /* Current option being processed */

/* -------------------------------------------------------------------------- */

/**
 * @brief Total options
 */
enum e_OptionIndex {
    OPT_INDEX_V = 0U,
    OPT_INDEX_V_LONG,
    OPT_INDEX_P,
    OPT_INDEX_P_LONG,
    OPT_INDEX_HELP,
    OPT_INDEX_HELP_LONG,
    OPT_INDEX_N,
    OPT_INDEX_N_LONG,
    OPT_INDEX_I,
    OPT_INDEX_I_LONG,
    OPT_INDEX_TTL,
    OPT_INDEX_TTL_LONG,
    OPT_INDEX_W,
    OPT_INDEX_W_LONG,
    OPT_INDEX_C,
    OPT_INDEX_C_LONG,
    OPT_INDEX_S,
    OPT_INDEX_S_LONG,
    OPT_INDEX_W_CAPS,
    OPT_INDEX_W_CAPS_LONG,

    OPTION_COUNT
};

/**
 * @brief Short options (single character) value
 */
enum e_ShortOptionFlag {
    FLAG_V = 'v',
    FLAG_P = 'p',
    FLAG_HELP_QM = '?',
    FLAG_N = 'n',
    FLAG_I = 'i',
    FLAG_W = 'w',
    FLAG_C = 'c',
    FLAG_S = 's',
    FLAG_W_CAPS = 'W',

    FLAG_LONG = 0,
    FLAG_END = -1,
};

/**
 * @brief Long options (multi-character)
 */
enum e_LongOptionIndex {
    LONG_OPT_INDEX_HELP = 0U,
    LONG_OPT_INDEX_VERBOSE,
    LONG_OPT_INDEX_NUMERIC,
    LONG_OPT_INDEX_TTL,
    LONG_OPT_INDEX_PATTERN,
    LONG_OPT_INDEX_TIMEOUT,
    LONG_OPT_INDEX_INTERVAL,
    LONG_OPT_INDEX_COUNT,
    LONG_OPT_INDEX_SIZE,
    LONG_OPT_INDEX_LINGER,

    LONG_OPTION_COUNT
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
FT_RESULT _set_flag(void* flag, FT_RESULT (*process_value)(char*, void*)) {
    if (flag == NULL) {
        log_debug("_set_flag", "improper address for flag");
        return FT_FAILURE;

    } else if (optarg == NULL) { /* optarg detailed above */
        log_error("bad value for `%c'", _current_option);
        return FT_FAILURE;

    } else if (process_value(optarg, flag) == FT_FAILURE) {
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

static
FT_RESULT _retrieve_options(const int arg_count, char* const* arg_values) {
    Options* options = &g_arguments.m_options;

    int long_option[LONG_OPTION_COUNT] = {
        [LONG_OPT_INDEX_HELP] = 0,
        [LONG_OPT_INDEX_VERBOSE] = 0,
        [LONG_OPT_INDEX_NUMERIC] = 0,
        [LONG_OPT_INDEX_TTL] = 0,
        [LONG_OPT_INDEX_PATTERN] = 0,
        [LONG_OPT_INDEX_TIMEOUT] = 0,
        [LONG_OPT_INDEX_INTERVAL] = 0,
        [LONG_OPT_INDEX_COUNT] = 0,
        [LONG_OPT_INDEX_SIZE] = 0,
        [LONG_OPT_INDEX_LINGER] = 0
    };

    struct option option_descriptors[OPTION_COUNT + 1] = {
        /* Short options */
        { "?",          no_argument,        NULL,                                   FLAG_HELP_QM },
        { "n",          no_argument,        NULL,                                   FLAG_N },
        { "v",          no_argument,        NULL,                                   FLAG_V },
        { "p",          required_argument,  NULL,                                   FLAG_P },
        { "w",          required_argument,  NULL,                                   FLAG_W },
        { "i",          required_argument,  NULL,                                   FLAG_I },
        { "c",          required_argument,  NULL,                                   FLAG_C },
        { "s",          required_argument,  NULL,                                   FLAG_S },
        { "W",          required_argument,  NULL,                                   FLAG_W_CAPS },
        /* Long options */
        { "help",       no_argument,        &long_option[LONG_OPT_INDEX_HELP],      1 },
        { "verbose",    no_argument,        &long_option[LONG_OPT_INDEX_VERBOSE],   1 },
        { "numeric",    no_argument,        &long_option[LONG_OPT_INDEX_NUMERIC],   1 },
        { "ttl",        required_argument,  &long_option[LONG_OPT_INDEX_TTL],       1 },
        { "pattern",    required_argument,  &long_option[LONG_OPT_INDEX_PATTERN],   1 },
        { "timeout",    required_argument,  &long_option[LONG_OPT_INDEX_TIMEOUT],   1 },
        { "interval",   required_argument,  &long_option[LONG_OPT_INDEX_INTERVAL],  1 },
        { "count",      required_argument,  &long_option[LONG_OPT_INDEX_COUNT],     1 },
        { "size",       required_argument,  &long_option[LONG_OPT_INDEX_SIZE],      1 },
        { "linger",     required_argument,  &long_option[LONG_OPT_INDEX_LINGER],    1 },
        { 0, 0, 0, 0 }
    };

    const char* short_options = "?vn" "p:w:i:c:s:W:";

    while (true) {
        _current_option = getopt_long(arg_count, arg_values, short_options, option_descriptors, NULL);

        switch (_current_option) {
            /* Long options */
            case FLAG_LONG:
                     if (long_option[LONG_OPT_INDEX_TTL]) { if (_set_flag((void*)&options->m_ttl, uchar_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_PATTERN]) {  if (_set_flag((void*)&options->m_pattern, hex_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_TIMEOUT]) {  if (_set_flag((void*)&options->m_timeout, int_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_INTERVAL]) {  if (_set_flag((void*)&options->m_interval, int_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_COUNT]) {  if (_set_flag((void*)&options->m_count, int_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_SIZE]) {  if (_set_flag((void*)&options->m_size, int_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_LINGER]) {  if (_set_flag((void*)&options->m_linger, int_check) == FT_FAILURE) { return FT_FAILURE; }; }
                else if (long_option[LONG_OPT_INDEX_NUMERIC]) { _enable_flag(&options->m_numeric); }
                else if (long_option[LONG_OPT_INDEX_VERBOSE]) { _enable_flag(&options->m_verbose); }
                else if (long_option[LONG_OPT_INDEX_HELP]) { _enable_flag(&options->m_help); return FT_SUCCESS; }
                /* Reset long option interception */
                for (u32 i = 0; i < LONG_OPTION_COUNT; ++i) { long_option[i] = 0; }
                break;

            /* Short options */
            case FLAG_HELP_QM:      if (optopt == 0) { _enable_flag(&options->m_help); return FT_SUCCESS; } else { return FT_FAILURE; }
            case FLAG_V:            _enable_flag(&options->m_verbose); break;
            case FLAG_N:            _enable_flag(&options->m_numeric); break;
            case FLAG_P:            if (_set_flag((void*)&options->m_pattern, hex_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_W:            if (_set_flag((void*)&options->m_timeout, int_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_I:            if (_set_flag((void*)&options->m_interval, int_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_C:            if (_set_flag((void*)&options->m_count, int_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_S:            if (_set_flag((void*)&options->m_size, int_check) == FT_FAILURE) { return FT_FAILURE; } break;
            case FLAG_W_CAPS:       if (_set_flag((void*)&options->m_linger, int_check) == FT_FAILURE) { return FT_FAILURE; } break;

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

    if (g_arguments.m_options.m_help == true) {
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

    log_debug("retrieve_arguments", "destination: `%s'", g_arguments.m_destination);

    return FT_SUCCESS;
}