#ifndef NETWORK_IO_H
# define NETWORK_IO_H

#include "typedefs.h"
#include <sys/types.h>

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
FT_RESULT   send_request(const char* msg);

/**
 * @brief Receive ICMP messages
 */
FT_RESULT   receive_msg(void);


FT_RESULT   echo(void);

#endif /* NETWORK_IO_H */