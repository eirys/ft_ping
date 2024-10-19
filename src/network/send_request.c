#include "network_io.h"

#include <netinet/ip.h> /* struct iphdr */
#include <netinet/ip_icmp.h> /* struct icmp */
#include <string.h> /* strlen */

#include "raw_socket.h"
#include "options.h"

/* -------------------------------------------------------------------------- */

#define BUF_SIZE            UINT16_MAX
#define IP_HEADER_SIZE      20  /* No options */
#define ICMP_HEADER_SIZE    8

/* -------------------------------------------------------------------------- */

/**
 * @brief Implementation is from Mike Muuss (`ping` public domain version).
 */
static inline
u16 _compute_checksum(const u16* addr, u16 data_len) {
    const u16*  it = addr;
    i32         bytes_to_process = data_len;

    u32         accumulator = 0U;

    /* Iterate over addr, accumulates 2 bytes at a time */
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
void _set_ip_header(char* buffer, u16 data_len) {
    struct iphdr* ip_header = (struct iphdr*)buffer;

    const u32 byte_count = IP_HEADER_SIZE / sizeof(i32);

    ip_header->version  = 4;                            /* IP version: 4 */
    ip_header->ihl      = byte_count;                   /* Size of IP header (byte count) */
    ip_header->tos      = 0x0;                          /* Common ToS */
    ip_header->tot_len  = htons(byte_count + data_len); /* Length of IP header + data */
    ip_header->id       = 0;                            /* Let the kernel set it */
    ip_header->frag_off = 0;                            /* No fragmentation offset */
    ip_header->ttl      = g_arguments.m_options.m_ttl;  /* By default, TTL set to UCHAR_MAX. */
    ip_header->protocol = IPPROTO_ICMP;                 /* ICMP protocol */
    ip_header->check    = 0;                            /* Let the kernel compute it */
    ip_header->saddr    = INADDR_ANY;                   /* Source bin ip : let the kernel set it */
    ip_header->daddr    = g_socket.m_ip;                /* Destination bin ip */
}

static inline
void _set_icmp_header(char* buffer, u16 data_len) {
    struct icmphdr* icmp_header = (struct icmphdr*)buffer;

    static u16      sequence = 0;

    icmp_header->type               = ICMP_ECHO;    /* ICMP Echo request */
    icmp_header->code               = 0;            /* No specific context */
    icmp_header->un.echo.id         = g_pid;        /* Aid in matching echo requests/replies */
    icmp_header->un.echo.sequence   = ++sequence;   /* Aid in matching echo requests/replies */

    icmp_header->checksum           = _compute_checksum((u16*)icmp_header, data_len);
}

/* -------------------------------------------------------------------------- */

// TODO lol
FT_RESULT   send_request(const char* msg) {
    static char send_buffer[BUF_SIZE];

    const u32 header_size = IP_HEADER_SIZE + ICMP_HEADER_SIZE;
    const u32 message_size = BUF_SIZE - header_size;

    u16 data_len = sizeof(u64) - ICMP_HEADER_SIZE;

    /* If pattern flag is set */
    if (msg != NULL)
        data_len = strlen(msg);

    _set_ip_header(send_buffer, data_len + ICMP_HEADER_SIZE);
    _set_icmp_header(send_buffer + IP_HEADER_SIZE, data_len);

    /* Fill with (random??) pattern */
    memset(send_buffer + header_size, 0b10100101 /* ??? */, message_size);
    if (msg != NULL)
        memcpy(send_buffer + header_size, msg, data_len);

    send(g_socket.m_fd, send_buffer, header_size + data_len, 0x0);

    return FT_SUCCESS;
}