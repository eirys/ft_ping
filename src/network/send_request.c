#include "network_io.h"

#include <netinet/ip.h> /* struct iphdr */
#include <netinet/ip_icmp.h> /* struct icmp */
#include <string.h> /* strlen */

#include "raw_socket.h"
#include "wrapper.h"
#include "options.h"

/* -------------------------------------------------------------------------- */

#define IP_HEADER_SIZE      20  /* No options */

#define ICMP_MSG_SIZE       64
#define ICMP_HEADER_SIZE    8
#define ICMP_DATA_SIZE      (ICMP_MSG_SIZE - ICMP_HEADER_SIZE)

#define BUF_SIZE            (ICMP_MSG_SIZE + IP_HEADER_SIZE)

/* -------------------------------------------------------------------------- */

/**
 * @brief Implementation is from Mike Muuss (`ping` public domain version).
 */
static inline
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

static inline
void _set_ip_header(struct iphdr* ip_header) {
    const u32 ihl = IP_HEADER_SIZE / sizeof(i32);

    ip_header->version  = 4U;                               /* IP version: 4 */
    ip_header->ihl      = ihl;                              /* Size of IP header (byte count) */
    ip_header->tos      = 0x0;                              /* Common ToS */
    ip_header->tot_len  = htons(BUF_SIZE);                  /* Length of IP header + data */
    ip_header->id       = htons(0U);                        /* Let the kernel set it */
    ip_header->frag_off = htons(0U);                        /* No fragmentation offset */
    ip_header->ttl      = g_arguments.m_options.m_ttl;      /* By default, TTL set to UCHAR_MAX. */
    ip_header->protocol = IPPROTO_ICMP;                     /* ICMP protocol */
    ip_header->saddr    = INADDR_ANY;                       /* Source bin ip : let the kernel set it */
    ip_header->daddr    = g_socket.m_ipv4->sin_addr.s_addr; /* Destination bin ip */

    ip_header->check    = _compute_checksum((u16*)ip_header, IP_HEADER_SIZE);
}

static inline
void _set_icmp_header(struct icmphdr* icmp_header) {
    static u16      sequence = 0;

    icmp_header->type               = ICMP_ECHO;    /* ICMP Echo request */
    icmp_header->code               = 0U;           /* No specific context */
    icmp_header->un.echo.id         = g_pid;        /* Aid in matching echo requests/replies */
    icmp_header->un.echo.sequence   = ++sequence;   /* Aid in matching echo requests/replies */

    icmp_header->checksum           = _compute_checksum((u16*)icmp_header, ICMP_MSG_SIZE);
}

/* -------------------------------------------------------------------------- */
#include <stdio.h>
FT_RESULT   send_request() {
    static u8       send_buffer[BUF_SIZE];

    struct iphdr*   ip_header = (struct iphdr*)send_buffer;
    struct icmphdr* icmp_header = (struct icmphdr*)(ip_header + 1);
    void*           message = (void*)(icmp_header + 1);

    _set_ip_header(ip_header);
    _set_icmp_header(icmp_header);

    Memset64(message, g_arguments.m_options.m_pattern, ICMP_DATA_SIZE / sizeof(u64));

    //debug
    u8 buf2[BUF_SIZE];
    memcpy(buf2, send_buffer, BUF_SIZE);
    // print
    for (int i = 0; i < BUF_SIZE; i++) {
        printf("%02x ", buf2[i]);
    }
    printf("\n");

    return Sendto(
        g_socket.m_fd,
        send_buffer,
        BUF_SIZE,
        0x0, /* No flags */
        (struct sockaddr*)g_socket.m_ipv4,
        sizeof(struct sockaddr_in));
}