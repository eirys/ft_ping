// #include "icmp.h"
#include <netdb.h>
#include <string.h>

struct addrinfo _resolve_ip(const char* destination) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    return hints;
}

// FT_RESULT   send_request(void) {

// }

// FT_RESULT   receive_response(void) {

// }

