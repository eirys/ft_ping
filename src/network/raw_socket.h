#ifndef SOCKET_H
# define SOCKET_H

#include <netinet/in.h> /* sockaddr_in */
#include "typedefs.h"

/* -------------------------------------------------------------------------- */
/*                                   STRUCT                                   */
/* -------------------------------------------------------------------------- */

typedef struct RawSocket {
    struct sockaddr_in* m_ipv4;
    int                 m_fd;           /* File descriptor */
    // i32                 m_ip;           /* Binary representation of the destination IP */
    // char*               m_ip_address;   /* IP address in str form (can be the same as destination) */
} RawSocket;

/* -------------------------------------------------------------------------- */
/*                                   GLOBALS                                  */
/* -------------------------------------------------------------------------- */

extern RawSocket g_socket;

/* -------------------------------------------------------------------------- */
/*                                 PROTOTYPES                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Create and open a raw socket, with extra information.
 */
FT_RESULT   create_raw_socket(const char* destination);

/**
 * @brief Close the raw socket and free allocated data.
 */
void        close_raw_socket(void);

#endif /* SOCKET_H */