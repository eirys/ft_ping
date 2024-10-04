#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "wrapper.h"
#include "log.h"

struct s_Options {
    i32     m_flag_a;
    i32     m_flag_b;
    char*   m_flag_c;
    i32     m_help;
};
typedef struct s_Options    Options;

enum e_OptionIndex {
    INDEX_FLAG_A = 0,
    INDEX_FLAG_B,
    INDEX_FLAG_C,
    INDEX_HELP,

    INDEX_COUNT
};
typedef enum e_OptionIndex  OptionIndex;

enum e_OptionFlag {
    FLAG_A = 'a',
    FLAG_B = 'b',
    FLAG_C = 'c',

    FLAG_UNKNOWN = '?'
};
typedef enum e_Options      Option;


static
FT_RESULT get_options(const int arg_count, const char* const* arg_values) {
    Options opts;

    opts.m_flag_a = 0;
    opts.m_flag_b = 0;
    opts.m_flag_c = NULL;
    opts.m_help = 0;

    struct option long_options[]= {
        { "flag_a",  no_argument,        0,              FLAG_A },
        { "flag_b",  no_argument,        0,              FLAG_B },
        { "flag_c",  required_argument,  0,              FLAG_C },
        { "help",    no_argument,        &opts.m_help,   1 },
        { 0, 0, 0, 0 }
    };

    int option_index = 0;
    int option_result = 0;

    while (true) {
        option_result = getopt_long(
            arg_count,
            (char* const*)arg_values,
            "abc:",
            long_options,
            &option_index);

        switch (option_result) {
            case -1: // No more options
                break;

            case FLAG_A:        opts.m_flag_a = 1; break;
            case FLAG_B:        opts.m_flag_b = 1; break;
            case FLAG_C:        opts.m_flag_c = optarg; break;

            case '?':
                printf("Unknown option: `%c' (%d)\n", (char)optopt, optopt);
                break;

            default:
                break;
        }

        ++option_index;
    }

    log_info("Options:\n");
    log_info("  -a: %d\n", opts.m_flag_a);
    log_info("  -b: %d\n", opts.m_flag_b);
    log_info("  -c: %s\n", opts.m_flag_c);
    log_info("  --help: %d\n", opts.m_help);

    return FT_SUCCESS;
}


int main(const int argc, const char* const* argv, const char* const* env) {
    get_options(argc, argv);
    (void)env;

    return 0;
}