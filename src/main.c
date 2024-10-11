#include <stdlib.h>

#include "options.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

#ifdef __DEBUG
static
void _debug_option() {
    log_info("Options:");
    log_info("  -a: %d", g_arguments.m_options.m_flag_a);
    log_info("  -b: %d", g_arguments.m_options.m_flag_b);
    log_info("  -c: %s", g_arguments.m_options.m_flag_c);
    log_info("  --help: %d", g_arguments.m_options.m_help);
}
#else
# define _debug_option() (void)0
#endif

static
void _show_help(const char* program_name) {
    log_info("\nUsage:");
    log_info("  %s [-SHORT_OPTION] <destination>", program_name);
    log_info("  %s [--LONG_OPTION]", program_name);
}

/* -------------------------------------------------------------------------- */

int main(i32 arg_count, char* const* arg_value) {
    if (retrieve_arguments(arg_count, arg_value) == FT_FAILURE) {
        _show_help(arg_value[0]);
        return EXIT_FAILURE;
    }



    if (arg_count == 1) {
        log_error("No arguments provided");
        return EXIT_FAILURE;
    }

    if (g_arguments.m_options.m_help) {
        _show_help(arg_value[0]);
        return EXIT_SUCCESS;
    }

    _debug_option();

    return EXIT_SUCCESS;
}