#include <stdlib.h> /* exit */
#include <unistd.h> /* getpid */

#include "options.h"
#include "log.h"
#include "raw_socket.h"
#include "network_io.h"
#include "set_signals.h"

/* --------------------------------- GLOBALS -------------------------------- */

pid_t g_pid;

/* -------------------------------------------------------------------------- */

// #ifdef __DEBUG
static
void _debug_option() {
    log_info("Options:");
    log_info("  --ttl: %d", g_arguments.m_options.m_ttl);
    log_info("  -v: %d", g_arguments.m_options.m_verbose);
    log_info("  -i: %d", g_arguments.m_options.m_interval);
    log_info("  -c: %u", g_arguments.m_options.m_count);
    log_info("  -W: %d", g_arguments.m_options.m_linger);
    log_info("  -n: %d", g_arguments.m_options.m_numeric);
    log_info("  -p: 0x%X (len: %lu)", g_arguments.m_options.m_pattern.content, g_arguments.m_options.m_pattern.length);
    log_info("  --help: %d", g_arguments.m_options.m_help);
}
// #endif

/* -------------------------------------------------------------------------- */

static
void _show_help(const char* program_name) {
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
    } else if (set_signals() == FT_FAILURE) {
        return FT_FAILURE;
    }

    /* Send first ping */
    log_info("FT_PING %s (%s): %u data bytes", g_arguments.m_destination, g_socket.m_ipv4_str, ICMP_PAYLOAD_SIZE);
    ping(42);

    if (wait_responses() == FT_FAILURE) {
        return FT_FAILURE;
    }

    close_raw_socket();
    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int main(int arg_count, char* const* arg_value) {
#ifndef __DEBUG //TODO remove
    if (_check_privileges() == FT_FAILURE) {
        return EXIT_FAILURE;
    }
#endif

    g_pid = getpid() & 0xFFff;

    if (retrieve_arguments(arg_count, arg_value) == FT_FAILURE) {
        return EXIT_FAILURE;
    }
    if (g_arguments.m_options.m_help) {
        _show_help(arg_value[0]);
    } else {
#ifdef __DEBUG
        _debug_option();
#endif
        if (_trigger() == FT_FAILURE)
            EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}