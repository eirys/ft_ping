#include "network_io.h"

#include <string.h> /* strlen */
#include <stdio.h> /* printf */

#include "raw_socket.h"
#include "wrapper.h"
#include "options.h"
#include "stats.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

u8* g_send_buffer = NULL;
u32 g_send_buffer_size = 0U;

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
void _set_ip_header(struct iphdr* ip_header, u32 packet_size) {
    ip_header->version  = 4U;                               /* IP version: 4 */
    ip_header->ihl      = IP_HEADER_SIZE / sizeof(i32);     /* Size of IP header (in 32-bit words) */
    ip_header->tos      = 0x0;                              /* Common ToS */
    ip_header->tot_len  = htons(packet_size);               /* Length of IP header + data */
    ip_header->id       = htons(42U);                       /* Random value */
    ip_header->frag_off = htons(0U);                        /* No fragmentation offset */
    ip_header->ttl      = g_arguments.m_options.m_ttl;      /* By default, TTL set to UCHAR_MAX. */
    ip_header->protocol = IPPROTO_ICMP;                     /* ICMP protocol */
    ip_header->saddr    = INADDR_ANY;                       /* Source bin ip : let the kernel set it */
    ip_header->daddr    = g_socket.m_ipv4->sin_addr.s_addr; /* Destination bin ip */
    ip_header->check    = 0;

    ip_header->check    = _compute_checksum((u16*)ip_header, IP_HEADER_SIZE);
}

static
void _set_icmp_header(struct icmphdr* icmp_header, u32 icmp_size) {
    /* Set it after filling payload with pattern for checksum */
    icmp_header->type               = ICMP_ECHO;                /* ICMP Echo request */
    icmp_header->code               = 0U;                       /* No specific context */
    icmp_header->un.echo.id         = g_pid;                    /* Aid in matching echo requests/replies */
    icmp_header->un.echo.sequence   = g_stats.m_packet_sent++;  /* Aid in matching echo requests/replies */
    icmp_header->checksum           = 0;

    icmp_header->checksum           = _compute_checksum((u16*)icmp_header, icmp_size);
}

static
FT_RESULT _set_payload(u8* payload, const Pattern* pattern, u32 payload_size) {
    const u32 value_length = pattern->length;

    /* Set pattern */
    for (u32 i = 0; i < payload_size; i += value_length) {
        for (u32 j = 0; j < value_length; j++) {
            payload[i + j] = pattern->raw[value_length - j - 1];
        }
    }

    /* Set timestamp */
    if (payload_size >= sizeof(struct timeval)) {
        if (Gettimeofday((struct timeval*)payload, NULL) == FT_FAILURE)
            return FT_FAILURE;
    }

    return FT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

void destroy_buffer() {
    if (g_send_buffer != NULL)
        Free(g_send_buffer);
}

FT_RESULT allocate_buffer() {
    if (g_send_buffer == NULL) {
        g_send_buffer_size = (IP_HEADER_SIZE + ICMP_HEADER_SIZE + g_arguments.m_options.m_size);
        g_send_buffer = Malloc(g_send_buffer_size);
        if (g_send_buffer == NULL) {
            return FT_FAILURE;
        }
    }
    return FT_SUCCESS;
}

FT_RESULT send_request() {
    struct iphdr*   ip = (struct iphdr*)g_send_buffer;
    struct icmphdr* icmp = (struct icmphdr*)(g_send_buffer + IP_HEADER_SIZE);
    u8*             payload = (u8*)(g_send_buffer + IP_HEADER_SIZE + ICMP_HEADER_SIZE);

    _set_ip_header(ip, g_send_buffer_size);
    if (_set_payload(payload, &g_arguments.m_options.m_pattern, g_arguments.m_options.m_size) == FT_FAILURE)
        return FT_FAILURE;
    _set_icmp_header(icmp, ICMP_HEADER_SIZE + g_arguments.m_options.m_size);

    return Sendto(
        g_socket.m_fd,
        g_send_buffer,
        g_send_buffer_size,
        0x0, /* No flags */
        (struct sockaddr*)g_socket.m_ipv4,
        sizeof(struct sockaddr_in));
}