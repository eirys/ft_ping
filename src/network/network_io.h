#ifndef NETWORK_IO_H
# define NETWORK_IO_H

#include <sys/types.h> /* pid_t */
#include <netinet/ip_icmp.h> /* struct icmphdr */
#include <netinet/ip.h> /* struct iphdr */
#include <netinet/in.h> /* struct sockaddr_in */
#include <sys/time.h> /* struct timeval */

#include "typedefs.h"

#define IP_HEADER_SIZE      20  /* No options */
#define ICMP_MSG_SIZE       64
#define ICMP_HEADER_SIZE    8

/* -------------------------------------------------------------------------- */
/*                                   STRUCTS                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Represents an ICMP packet.
 */
typedef struct s_Packet {
    struct iphdr*       m_ip_header;
    struct icmphdr*     m_icmp_header;
    u8*                 m_icmp_payload;
    u32                 m_size;
    u32                 m_icmp_size;
    u32                 m_ip_header_size;
    struct timeval      m_timestamp;
    struct sockaddr_in  m_src;
} Packet;

/* -------------------------------------------------------------------------- */
/*                                   GLOBALS                                  */
/* -------------------------------------------------------------------------- */

extern pid_t    g_pid;
extern u8*      g_send_buffer;
extern u32      g_send_buffer_size;

/* -------------------------------------------------------------------------- */
/*                                 PROTOTYPES                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send an ICMP echo request to the given destination.
 */
FT_RESULT   send_request();

/**
 * @brief Wait for an ICMP response.
 */
FT_RESULT   wait_responses();

FT_RESULT   allocate_buffer();
void        destroy_buffer();

#endif /* NETWORK_IO_H */