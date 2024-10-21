#include "raw_socket.h"

#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#include "log.h"
#include "wrapper.h"

/* -------------------------------------------------------------------------- */

RawSocket  g_socket = {
    .m_fd = -1,
    .m_ipv4 = NULL,
    // .m_ip = 0xFFffFFff,
    // .m_ip_address = NULL
};

/* -------------------------------------------------------------------------- */

static struct addrinfo* _destination_info = NULL;

static inline
struct addrinfo* _resolve_ip(const char* destination) {
    struct addrinfo* output;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          /* Allow IPv4 */
    hints.ai_socktype = SOCK_RAW;       /* Raw socket */
    hints.ai_protocol = IPPROTO_ICMP;   /* IMCPv4 */

    int result = getaddrinfo(destination, NULL, &hints, &output);
    if (result != 0) {
        log_error("getaddrinfo failed: %s", gai_strerror(result));
        return NULL;
    }

    return output;
}

static
FT_RESULT _destroy_malformed_data(void) {
    if (_destination_info != NULL) {
        freeaddrinfo(_destination_info);
    }

    close_raw_socket();

    return FT_FAILURE;
}

/* -------------------------------------------------------------------------- */

FT_RESULT create_raw_socket(const char* destination) {
    _destination_info = _resolve_ip(destination);
    if (_destination_info == NULL) {
        return _destroy_malformed_data();
    }

    /* Open raw socket */
    g_socket.m_fd = Socket(_destination_info->ai_family, _destination_info->ai_socktype, _destination_info->ai_protocol);
    if (g_socket.m_fd == -1) {
        return _destroy_malformed_data();
    }

    /* Convert binary IPv4 to string */
    g_socket.m_ipv4 = Malloc(sizeof(struct sockaddr_in));
    if (g_socket.m_ipv4 == NULL) {
        return _destroy_malformed_data();
    }
    *g_socket.m_ipv4 = *(struct sockaddr_in*)_destination_info->ai_addr;

    // char ip[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &ipv4->sin_addr, ip, INET_ADDRSTRLEN);

    // g_socket.m_ip_address = Strdup(ip);
    // if (g_socket.m_ip_address == NULL) {
    //     return _destroy_malformed_data();
    // }

    /* IP Header: Tell the kernel that we build and include our own IP header */
    const i32 option_value = 1;
    setsockopt(g_socket.m_fd, IPPROTO_IP, IP_HDRINCL, &option_value, sizeof(option_value));

    freeaddrinfo(_destination_info);
    _destination_info = NULL;

    return FT_SUCCESS;
}

void close_raw_socket(void) {
    if (g_socket.m_fd != -1) {
        Close(g_socket.m_fd);
    }
    if (g_socket.m_ipv4 != NULL) {
        Free(g_socket.m_ipv4);
    }
}