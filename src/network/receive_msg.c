#include "network_io.h"

#include <netinet/ip.h> /* struct iphdr */
#include <netinet/ip_icmp.h> /* struct icmp */
#include <string.h> /* NULL */
#include <arpa/inet.h> /* inet_ntop */

#include "options.h"
#include "raw_socket.h"
#include "wrapper.h"
#include "log.h"

#define BUF_SIZE    32768 /* Large to handle replies overflows */

/* -------------------------------------------------------------------------- */

static struct sockaddr_in packet_src;

/* -------------------------------------------------------------------------- */

/**
 * @brief Receive an IPv4 packet.
 */
static
FT_RESULT _receive_packet(u8* buffer, u32* size) {
    struct sockaddr raw_src;
    socklen_t src_len = sizeof(raw_src);

    *size = Recvfrom(g_socket.m_fd, buffer, *size, 0x0, &raw_src, &src_len);
    if (*size < 0)
        return FT_FAILURE;

    packet_src = *(struct sockaddr_in*)&raw_src;

    if (packet_src.sin_family != AF_INET) {
        log_error("received packet from unknown family");
        return FT_FAILURE;
    }
    return FT_SUCCESS;
}

/**
 * @brief Filter properly formed ICMPv4 packets.
 */
static
u8* _filter_icmp(const u8* packet, u32* packet_size) {
    const struct iphdr*   ip_header = (struct iphdr*)packet;

    /* Not ICMPv4 */
    if (ip_header->protocol != IPPROTO_ICMP) {
        log_debug("_filter_icmp", "not an ICMP message");
        return NULL;
    }

    const u32 header_length = ip_header->ihl * 4;
    *packet_size -= header_length;

    if (*packet_size < ICMP_HEADER_SIZE) {
        log_error("packet malformed");
        return NULL;
    }

    return (u8*)(packet + header_length);
}

static
void _substract_tv(struct timeval* out, const struct timeval* in) {
    const long int ms = (out->tv_usec -= in->tv_usec);
    if (ms < 0) {
        out->tv_sec -= 1; /* Result is 1 second */
        out->tv_usec += (double)10e6;
    }
    out->tv_sec -= in->tv_sec;
}

static
FT_RESULT _process_message(const u8* icmp_message, const u32 packet_size, struct timeval* current_tv) {
    const struct icmphdr*   icmp_header = (struct icmphdr*)(icmp_message);
    const u8*               payload = (u8*)(icmp_message + ICMP_HEADER_SIZE);

    if (icmp_header->type == ICMP_ECHOREPLY) {
        /* Display packet data */
        if (packet_size < sizeof(struct timeval)) {
            log_error("no timestamp in received packet");
            return FT_FAILURE;
        } else {
            const struct timeval* received_tv = (struct timeval*)payload;
            _substract_tv(current_tv, received_tv);

            char src_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &packet_src.sin_addr, src_ip, INET_ADDRSTRLEN);

            log_info(
                "%u bytes from %s: seq=%u rtt=%.3f",
                packet_size,
                src_ip,
                icmp_header->un.echo.sequence,
                (current_tv->tv_sec * (double)10e3 + current_tv->tv_usec / (double)10e3));

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

    while (true) {
        /* Timeout */
        u32 message_length = BUF_SIZE;

        if (_receive_packet(message, &message_length) == FT_FAILURE)
            return FT_FAILURE;

        struct timeval current_time;
        if (Gettimeofday(&current_time, NULL) == FT_FAILURE)
            return FT_FAILURE;

        const u8* icmp_packet = _filter_icmp(message, &message_length);
        if (icmp_packet == NULL)
            return FT_FAILURE;
            // continue;

        if (_process_message(icmp_packet, message_length, &current_time) == FT_FAILURE)
            return FT_FAILURE;
    }

    return FT_SUCCESS;
}