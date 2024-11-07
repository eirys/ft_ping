#include <unistd.h>
#include <stdlib.h>

#include "network_io.h"
#include "options.h"
#include "wrapper.h"
#include "raw_socket.h"
#include "stats.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

static struct sigaction _old_alarm;
static struct sigaction _old_sigint;

/* -------------------------------------------------------------------------- */

void _reset_signals(void) {
    if (Sigaction(SIGALRM, &_old_alarm, NULL) == FT_FAILURE) {
        log_error("failed to reset alarm");
    }
    if (Sigaction(SIGINT, &_old_sigint, NULL) == FT_FAILURE) {
        log_error("failed to reset sigint");
    }
}

static
void _cleanup(void) {
    _reset_signals();
    destroy_raw_socket();
    destroy_buffer();
}

static
long int _elapsed(const struct timeval* start) {
    struct timeval now;
    if (Gettimeofday(&now, NULL) == FT_FAILURE) {
        _cleanup();
        exit(EXIT_FAILURE);
    }
    long int seconds = now.tv_sec - start->tv_sec;
    long int microseconds = now.tv_usec - start->tv_usec;
    if (microseconds < 0) {
        seconds -= 1; /* Result is 1 second */
    }
    return seconds;
}

/* -------------------------------------------------------------------------- */

/**
 * @brief Handler for SIGINT signal: stop the program gracefully.
 */
void stop(__attribute__((unused)) int signal) {
    _cleanup();
    display_stats();
    exit(EXIT_SUCCESS);
}

/**
 * @brief Handler for SIGALRM signal: send a ping.
 */
void ping(__attribute__((unused)) int signal) {
    static struct timeval start = {0, 0};

    if (start.tv_sec == 0 && start.tv_usec == 0) {
        if (Gettimeofday(&start, NULL) == FT_FAILURE) {
            _cleanup();
            exit(EXIT_FAILURE);
        }
    }

    if (send_request() == FT_FAILURE) {
        _cleanup();
        exit(EXIT_FAILURE);
    }

    g_stats.m_packet_sent += 1;

    if (_elapsed(&start) >= (long int)g_arguments.m_options.m_timeout) {
        stop(21);
    }

    if (g_stats.m_packet_sent < g_arguments.m_options.m_count) {
        alarm(g_arguments.m_options.m_interval);
    }
}

/**
 * @brief Enable signals to handle alarm (ping) and interrupt.
 */
FT_RESULT set_signals(void) {
    if (Sigaction(SIGALRM, NULL, &_old_alarm) == FT_FAILURE) {
        log_error("failed to save old alarm");
        return FT_FAILURE;
    }

    if (Sigaction(SIGINT, NULL, &_old_sigint) == FT_FAILURE) {
        log_error("failed to save old sigint");
        return FT_FAILURE;
    }

    struct sigaction    new_alarm;
    sigemptyset(&new_alarm.sa_mask);
    new_alarm.sa_flags = SA_RESTART;
    new_alarm.sa_handler = ping;
    if (Sigaction(SIGALRM, &new_alarm, NULL) == FT_FAILURE) {
        log_error("failed to set new alarm");
        return FT_FAILURE;
    }

    struct sigaction    new_sigint;
    sigemptyset(&new_sigint.sa_mask);
    new_sigint.sa_flags = SA_RESTART;
    new_sigint.sa_handler = stop;
    if (Sigaction(SIGINT, &new_sigint, NULL) == FT_FAILURE) {
        log_error("failed to set new sigint");
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}