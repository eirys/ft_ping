#ifndef NETWORK_IO_H
# define NETWORK_IO_H

#include "typedefs.h"
#include <sys/types.h>

#define IP_HEADER_SIZE      20  /* No options */
#define ICMP_MSG_SIZE       64
#define ICMP_HEADER_SIZE    8
#define ICMP_PAYLOAD_SIZE   (ICMP_MSG_SIZE - ICMP_HEADER_SIZE)

/* -------------------------------------------------------------------------- */
/*                                   GLOBALS                                  */
/* -------------------------------------------------------------------------- */

extern pid_t    g_pid;

/* -------------------------------------------------------------------------- */
/*                                 PROTOTYPES                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send an ICMP echo request to the given destination.
 */
FT_RESULT   send_request(void);

/**
 * @brief Wait for an ICMP response.
 */
FT_RESULT   wait_responses(void);//const u8* message, const u32 message_len);

#endif /* NETWORK_IO_H */