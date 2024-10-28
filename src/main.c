#include <stdlib.h> /* exit */
#include <unistd.h> /* getpid */
#include <signal.h> /* sigqueue */

#include "options.h"
#include "log.h"
#include "raw_socket.h"
#include "network_io.h"
#include "set_signals.h"
#include "wrapper.h"

/* --------------------------------- GLOBALS -------------------------------- */

pid_t   g_pid;

/* -------------------------------------------------------------------------- */

static
void _show_help(const char* program_name) {
    //TODO + readme

    log_info("Usage:");
    log_info("  %s [-SHORT_OPTION] <destination>", program_name);
    log_info("  %s [--LONG_OPTION]", program_name);
}

static
FT_RESULT _check_privileges() {
    if (getuid() != 0) {
        log_error("root privileges required");
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

static
FT_RESULT _trigger(void) {
    /* Setup */
    if (create_raw_socket(g_arguments.m_destination) == FT_FAILURE) {
        return FT_FAILURE;
    }
    if (set_signals() == FT_FAILURE) {
        return FT_FAILURE;
    }
    if (allocate_buffer() == FT_FAILURE) {
        return FT_FAILURE;
    }

    /* Send first ping */
    if (g_arguments.m_options.m_verbose)
        log_info("FT_PING %s (%s): %u data bytes, id 0x%04x = %d",
        g_arguments.m_destination,
        g_socket.m_ipv4_str,
        g_arguments.m_options.m_size,
        g_pid,
        g_pid);
    else
        log_info("FT_PING %s (%s): %u data bytes",
        g_arguments.m_destination,
        g_socket.m_ipv4_str,
        g_arguments.m_options.m_size);

    ping(42);

    if (wait_responses() == FT_FAILURE) {
        return FT_FAILURE;
    }

//TODO display stats

    close_raw_socket();
    destroy_buffer();
    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int main(int arg_count, char* const* arg_value) {
    if (_check_privileges() == FT_FAILURE) {
        return EXIT_FAILURE;
    }
    if (retrieve_arguments(arg_count, arg_value) == FT_FAILURE) {
        return EXIT_FAILURE;
    }

    g_pid = getpid() & 0xFFff;

    if (g_arguments.m_options.m_help) {
        _show_help(arg_value[0]);
    } else {
        if (_trigger() == FT_FAILURE)
            EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}