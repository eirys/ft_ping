#include <stdlib.h> /* exit */
#include <unistd.h> /* getpid */

#include "options.h"
#include "log.h"
#include "network_io.h"

/* --------------------------------- GLOBALS -------------------------------- */

pid_t g_pid;

/* -------------------------------------------------------------------------- */

#ifdef __DEBUG

static
void _debug_option() {
    log_info("Options:");
    log_info("  -t: %d", g_arguments.m_options.m_ttl);
    log_info("  -v: %d", g_arguments.m_options.m_verbose);
    log_info("  -p: %s", g_arguments.m_options.m_pattern);
    log_info("  --help: %d", g_arguments.m_options.m_help);
}

#else

# define _debug_option() (void)0

#endif

/* -------------------------------------------------------------------------- */

static
void _show_help(const char* program_name) {
    log_info("\nUsage:");
    log_info("  %s [-SHORT_OPTION] <destination>", program_name);
    log_info("  %s [--LONG_OPTION]", program_name);
}

static
int _exit_cleanup(int exit_value) {
    destroy_options();
    return exit_value;
}

/* -------------------------------------------------------------------------- */

int main(int arg_count, char* const* arg_value) {
    g_pid = getpid() & 0xFFff;

    if (retrieve_arguments(arg_count, arg_value) == FT_FAILURE) {
        _exit_cleanup(EXIT_FAILURE);
    }

    if (g_arguments.m_options.m_help) {
        _show_help(arg_value[0]);
        return _exit_cleanup(EXIT_SUCCESS);
    } else {
        _debug_option();

        echo();
    }

    return _exit_cleanup(EXIT_SUCCESS);
}