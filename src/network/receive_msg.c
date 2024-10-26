#include "network_io.h"

#include <netinet/ip.h> /* struct iphdr */
#include <netinet/ip_icmp.h> /* struct icmp */
#include <string.h> /* NULL */
#include <arpa/inet.h> /* inet_ntop */
#include <sys/param.h> /* MIN MAX */

#include "options.h"
#include "raw_socket.h"
#include "wrapper.h"
#include "log.h"
#include "stats.h"

#define BUF_SIZE    32768 /* Large to handle replies overflows */

/* -------------------------------------------------------------------------- */

typedef struct s_Packet {
    struct iphdr*       m_ip_header;
    struct icmphdr*     m_icmp_header;
    struct sockaddr_in  m_src;
    struct timeval      m_reception_time;
    u8*                 m_payload;
    u32                 m_size;
    u32                 m_icmp_size;
} Packet;

/* -------------------------------------------------------------------------- */

/**
 * @brief Receive an IPv4 packet.
 */
static
FT_RESULT _receive_packet(u8* buffer, Packet* packet) {
    struct sockaddr raw_src;
    socklen_t src_len = sizeof(raw_src);

    packet->m_size = Recvfrom(g_socket.m_fd, buffer, BUF_SIZE, 0x0, &raw_src, &src_len);
    if (packet->m_size < 0)
        return FT_FAILURE;

    packet->m_src = *(struct sockaddr_in*)&raw_src;

    if (packet->m_src.sin_family != AF_INET) {
        log_error("received packet from unknown family");
        return FT_FAILURE;
    }
    return FT_SUCCESS;
}

/**
 * @brief Filter properly formed ICMPv4 packets.
 */
static
FT_RESULT _filter_icmp(const u8* raw_packet, Packet* packet) {
    const struct iphdr*   ip_header = (struct iphdr*)raw_packet;

    /* Not ICMPv4 */
    if (ip_header->protocol != IPPROTO_ICMP) {
        log_debug("_filter_icmp", "not an ICMP message");
        return FT_FAILURE;
    }

    const u32 header_length = ip_header->ihl * sizeof(u32);
    if (packet->m_size - header_length < ICMP_HEADER_SIZE) {
        log_error("packet malformed");
        return FT_FAILURE;
    }

    packet->m_icmp_size = packet->m_size - header_length;
    packet->m_ip_header = (struct iphdr*)raw_packet;
    packet->m_icmp_header = (struct icmphdr*)(raw_packet + header_length);
    packet->m_payload = (u8*)(raw_packet + header_length + ICMP_HEADER_SIZE);

    return FT_SUCCESS;
}

static
double _compute_rtt(const struct timeval* t1, const struct timeval* t2) {
    long int seconds = t1->tv_sec - t2->tv_sec;
    long int microseconds = t1->tv_usec - t2->tv_usec;
    if (microseconds < 0) {
        seconds -= 1; /* Result is 1 second */
        microseconds += 1e7;
    }
    return (seconds * (double)1e3 + microseconds / (double)1e3);
}

static
FT_RESULT _process_message(const Packet* packet) {
    if (packet->m_icmp_header->type == ICMP_ECHOREPLY) {
        /* Display packet data */
        if (packet->m_icmp_size < sizeof(struct timeval)) {
            log_error("no timestamp in received packet");
            return FT_FAILURE;
        } else {
            const struct timeval*   sent_tv = (struct timeval*)packet->m_payload;
            const double            rtt = _compute_rtt(&packet->m_reception_time, sent_tv);

            char src_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &packet->m_src.sin_addr, src_ip, INET_ADDRSTRLEN);

            log_info(
                "%u bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms",
                packet->m_icmp_size,
                src_ip,
                packet->m_icmp_header->un.echo.sequence,
                packet->m_ip_header->ttl,
                rtt);

            g_stats.m_min_rtt = MIN(rtt, g_stats.m_min_rtt);
            g_stats.m_max_rtt = MAX(rtt, g_stats.m_max_rtt);
            g_stats.m_total_rtt += rtt;
            g_stats.m_total_rtt_square += rtt * rtt;

            return FT_SUCCESS;
        }

        // if (g_arguments.m_options.m_verbose)
            // do extra stuff

    } else if (g_arguments.m_options.m_verbose) {
        log_debug("_process_message", "received different ICMP packet than expected");
        // do stuff
    }

    return FT_FAILURE;
}

/* -------------------------------------------------------------------------- */

FT_RESULT wait_response() {
    u8 message[BUF_SIZE];

    struct timeval timeout;
    timeout.tv_sec = g_arguments.m_options.m_linger;
    timeout.tv_usec = 0;

    fd_set listen_fds;
    FD_ZERO(&listen_fds);
    FD_SET(g_socket.m_fd, &listen_fds);

    while (true) {
        int fds = Select(g_socket.m_fd + 1, &listen_fds, NULL, NULL, &timeout);
        if (fds == -1)
            return FT_FAILURE;
        else if (fds == 0) /* Timeout */
            return FT_SUCCESS;

        Packet  packet;

        if (_receive_packet(message, &packet) == FT_FAILURE)
            return FT_FAILURE;

        if (Gettimeofday(&packet.m_reception_time, NULL) == FT_FAILURE)
            return FT_FAILURE;

        if (_filter_icmp(message, &packet) == FT_FAILURE)
            continue;

        if (_process_message(&packet) == FT_FAILURE)
            return FT_FAILURE;
    }

    return FT_SUCCESS;
}