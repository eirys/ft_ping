#include "network_io.h"

#include <netinet/ip.h> /* struct iphdr */
#include <netinet/ip_icmp.h> /* struct icmp */
#include <string.h> /* NULL */

#include "options.h"
#include "raw_socket.h"
#include "wrapper.h"
#include "log.h"

#define BUF_SIZE    32768 /* Large to handle replies overflows */

/* -------------------------------------------------------------------------- */

FT_RESULT _receive_packet(u8* buffer, u32* size) {
    struct sockaddr raw_src;
    socklen_t src_len = sizeof(raw_src);

    *size = Recvfrom(g_socket.m_fd, buffer, *size, 0x0, &raw_src, &src_len);
    if (*size < 0)
        return FT_FAILURE;

    struct sockaddr_in* src = (struct sockaddr_in*)&raw_src;
    if (src->sin_family != AF_INET) {
        log_error("received packet from unknown family");
        return FT_FAILURE;
    }

    return FT_SUCCESS;
}

u8* _filter_icmp(const u8* packet, u32* packet_size) {
    const struct iphdr*   ip_header = (struct iphdr*)packet;

    /* Not ICMPv4 */
    if (ip_header->protocol != IPPROTO_ICMP) {
        return NULL;
    }

    const u32 header_length = ip_header->ihl * 4;
    *packet_size -= header_length;

    return (u8*)(ip_header + header_length);
}

FT_RESULT _process_message(const u8* icmp_message, const u32 packet_size) {
    if (packet_size < ICMP_HEADER_SIZE) {
        log_error("received packet is too small");
        return FT_FAILURE;
    }

    const struct icmphdr* icmp_header = (struct icmphdr*)(icmp_message);
    const u8*             payload = (void*)(icmp_header + 1);

    if (icmp_header->type == ICMP_ECHOREPLY) {
        log_info("received ICMP echo reply");
        // if (g_arguments.m_options.m_verbose)
            // do stuff

    } else if (g_arguments.m_options.m_verbose) {
        log_info("received sth else");
        // do stuff
    }

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

FT_RESULT wait_response() {
    return FT_SUCCESS;

    u8 message[BUF_SIZE];

    /* Waiting for 10 seconds */
    struct timeval  timeout;
    timeout.tv_sec = g_arguments.m_options.m_linger;
    timeout.tv_usec = 0;

    fd_set listen_set;
    FD_ZERO(&listen_set);
    FD_SET(g_socket.m_fd, &listen_set);

    while (true) {
        /* Timeout */
        int fds_ready  = Select(g_socket.m_fd, &listen_set, NULL, NULL, &timeout);
        if (fds_ready <= 0) /* Timeout or error */
            return FT_FAILURE;

        u32 message_length = BUF_SIZE;

        if (_receive_packet(message, &message_length) == FT_FAILURE)
            return FT_FAILURE;

        const u8* icmp_packet = _filter_icmp(message, &message_length);
        if (icmp_packet == NULL)
            continue;

        if (_process_message(icmp_packet, message_length) == FT_FAILURE)
            return FT_FAILURE;
    }

    return FT_SUCCESS;
}