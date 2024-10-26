#include "network_io.h"

#include <netinet/ip.h> /* struct iphdr */
#include <netinet/ip_icmp.h> /* struct icmp */
#include <string.h> /* strlen */

#include "raw_socket.h"
#include "wrapper.h"
#include "options.h"
#include "stats.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

#define BUF_SIZE            (ICMP_MSG_SIZE + IP_HEADER_SIZE)

/* -------------------------------------------------------------------------- */

/**
 * @brief Implementation is from Mike Muuss (`ping` public domain version).
 */
static
u16 _compute_checksum(const u16* addr, u16 data_len) {
    i32         bytes_to_process = (i32)data_len;

    /* Iterate over addr, accumulates 2 bytes at a time */
    const u16*  it = addr;
    u32         accumulator = 0U;

    while (bytes_to_process > 1) {
        accumulator += *it++;
        bytes_to_process -= 2;
    }

    /* Process eventual remaining byte */
    if (bytes_to_process == 1) {
        accumulator += *(u8*)it;
    }

    /* Add carry from the top 16 bits */
    accumulator = (accumulator >> 16) + (accumulator & 0xFFff);

    /* Handle potential extra carry */
    accumulator += (accumulator >> 16);

    /* Return one's complement */
    return ~(accumulator);
}

static
void _set_ip_header(struct iphdr* ip_header) {
    ip_header->version  = 4U;                               /* IP version: 4 */
    ip_header->ihl      = IP_HEADER_SIZE / sizeof(i32);     /* Size of IP header (in 32-bit words) */
    ip_header->tos      = 0x0;                              /* Common ToS */
    ip_header->tot_len  = htons(BUF_SIZE);                  /* Length of IP header + data */
    ip_header->id       = htons(0U);                        /* Let the kernel set it */
    ip_header->frag_off = htons(0U);                        /* No fragmentation offset */
    ip_header->ttl      = g_arguments.m_options.m_ttl;      /* By default, TTL set to UCHAR_MAX. */
    ip_header->protocol = IPPROTO_ICMP;                     /* ICMP protocol */
    ip_header->saddr    = INADDR_ANY;                       /* Source bin ip : let the kernel set it */
    ip_header->daddr    = g_socket.m_ipv4->sin_addr.s_addr; /* Destination bin ip */
    ip_header->check    = 0;                                /* Let the kernel set it */
}

static
void _set_icmp_header(struct icmphdr* icmp_header) {
    /* Set it after filling payload with pattern for checksum */
    icmp_header->type               = ICMP_ECHO;                /* ICMP Echo request */
    icmp_header->code               = 0U;                       /* No specific context */
    icmp_header->un.echo.id         = g_pid;                    /* Aid in matching echo requests/replies */
    icmp_header->un.echo.sequence   = g_stats.m_packet_sent++;  /* Aid in matching echo requests/replies */
    icmp_header->checksum           = 0;

    icmp_header->checksum           = _compute_checksum((u16*)icmp_header, ICMP_MSG_SIZE);
}

static
FT_RESULT _set_payload(u8* dest, const Pattern* pattern, u32 dst_size) {
    const u8* src = (u8*)&pattern->content;
    const u32 value_length = pattern->length;

    /* Set pattern */
    for (u32 i = 0; i < dst_size; i += value_length) {
        for (u32 j = 0; j < value_length; j++) {
#if __BIG_ENDIAN
            dest[i + j] = src[value_length - j - 1];
 #elif __LITTLE_ENDIAN
            dest[i + j] = src[j];
#endif
        }
    }

    /* Set timestamp */
    if (Gettimeofday((struct timeval*)dest, NULL) == FT_FAILURE)
        return FT_FAILURE;

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

FT_RESULT   send_request() {
    u8              send_buffer[BUF_SIZE];

    struct iphdr*   ip_header = (struct iphdr*)send_buffer;
    struct icmphdr* icmp_header = (struct icmphdr*)(ip_header + 1);
    u8*             payload = (void*)(icmp_header + 1);

    /* IPv4 content */
    _set_ip_header(ip_header);

    /* ICMP content */
    if (_set_payload(payload, &g_arguments.m_options.m_pattern, ICMP_PAYLOAD_SIZE) == FT_FAILURE)
        return FT_FAILURE;
    _set_icmp_header(icmp_header);


    return Sendto(
        g_socket.m_fd,
        send_buffer,
        BUF_SIZE,
        0x0, /* No flags */
        (struct sockaddr*)g_socket.m_ipv4,
        sizeof(struct sockaddr_in));
}