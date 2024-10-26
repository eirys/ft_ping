#include <unistd.h>

#include "network_io.h"
#include "options.h"
#include "wrapper.h"
#include "raw_socket.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

static struct sigaction old_alarm;

/* -------------------------------------------------------------------------- */

void ping(__attribute__((unused)) int signal) {
    send_request();

    alarm(g_arguments.m_options.m_interval);
}

FT_RESULT set_alarm(void) {
    if (Sigaction(SIGALRM, NULL, &old_alarm) == FT_FAILURE) {
        log_error("failed to save old alarm");
        return FT_FAILURE;
    }

    struct sigaction    new_sig;
    sigemptyset(&new_sig.sa_mask);
    new_sig.sa_flags = SA_RESTART;
    new_sig.sa_handler = ping;
    if (Sigaction(SIGALRM, &new_sig, NULL) == FT_FAILURE) {
        log_error("failed to set new alarm");
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

FT_RESULT reset_alarm(void) {
    log_debug("reset_alarm", "resetting alarm");
    if (Sigaction(SIGALRM, &old_alarm, NULL) == FT_FAILURE) {
        log_error("failed to reset alarm");
        return FT_FAILURE;
    }
    return FT_SUCCESS;
}
