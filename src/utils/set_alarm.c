#include <signal.h>
#include <unistd.h>

#include "network_io.h"

static
void _alarm_handler(__attribute__((unused)) int signal) {
    send_request();

    alarm(1);
}