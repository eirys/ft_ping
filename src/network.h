#ifndef NETWORK_H
# define NETWORK_H

#include "typedefs.h"

// Send an ICMP request to the given destination
FT_RESULT   send_request(void);

// Receive the ICMP response
FT_RESULT   receive_response(void);

#endif // NETWORK_H