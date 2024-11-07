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
    log_info("Usage: %s [OPTION] <destination>", program_name);
    log_info("Send ICMP ECHO_REQUEST packets to network hosts\n");
    log_info("Options:");
    log_info("  -p, --pattern <pattern>     Pattern to fill ICMP payload");
    log_info("  -w, --timeout <timeout>     Time to wait for a response");
    log_info("  -i, --interval <interval>   Interval between each packet");
    log_info("  -c, --count <count>         Number of packets to send");
    log_info("  -s, --size <size>           Size of the ICMP payload");
    log_info("  --ttl <ttl>                 Time to live");
    log_info("  -n, --numeric               Display numeric addresses");
    log_info("  -v, --verbose               Verbose output");
    log_info("  -?, --help                  Display this help and exit");
}

static
FT_RESULT _check_privileges() {
    if (getuid() != 0) {
        log_error("root privileges required");
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

/**
 * @brief Trigger the ping program.
 */
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

    stop(42);

    return FT_SUCCESS;
}

static
void _cleanup(void) {
    destroy_raw_socket();
    destroy_buffer();
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
    } else if (_trigger() == FT_FAILURE) {
        _cleanup();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}