#include <unistd.h>
#include <stdlib.h>

#include "network_io.h"
#include "options.h"
#include "wrapper.h"
#include "raw_socket.h"
#include "stats.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

static struct sigaction old_alarm;
static struct sigaction old_sigint;

/* -------------------------------------------------------------------------- */

void ping(__attribute__((unused)) int signal) {
    // TODO stats
    send_request();

    alarm(g_arguments.m_options.m_interval);
}

FT_RESULT reset_signals(void) {
    if (Sigaction(SIGALRM, &old_alarm, NULL) == FT_FAILURE) {
        log_error("failed to reset alarm");
        return FT_FAILURE;
    } else if (Sigaction(SIGINT, &old_sigint, NULL) == FT_FAILURE) {
        log_error("failed to reset sigint");
        return FT_FAILURE;
    }
    return FT_SUCCESS;
}

void stop(__attribute__((unused)) int signal) {
    display_stats();

    reset_signals();
    close_raw_socket();

    exit(EXIT_SUCCESS);
}

FT_RESULT set_signals(void) {
    if (Sigaction(SIGALRM, NULL, &old_alarm) == FT_FAILURE) {
        log_error("failed to save old alarm");
        return FT_FAILURE;
    } else if (Sigaction(SIGINT, NULL, &old_sigint) == FT_FAILURE) {
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