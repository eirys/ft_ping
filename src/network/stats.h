#ifndef STATS_H
# define STATS_H

#include "typedefs.h"
#include <sys/time.h>

/* -------------------------------------------------------------------------- */
/*                                   STRUCTS                                  */
/* -------------------------------------------------------------------------- */

typedef struct s_PingStats {
    struct timeval  m_start;
    double          m_min_rtt;
    double          m_max_rtt;
    double          m_total_rtt;
    double          m_total_rtt_square;
    u32             m_packet_sent;
    u32             m_packet_received;
} PingStats;

/* -------------------------------------------------------------------------- */
/*                                   GLOBALS                                  */
/* -------------------------------------------------------------------------- */

extern PingStats g_stats;

/* -------------------------------------------------------------------------- */
/*                                 PROTOTYPES                                 */
/* -------------------------------------------------------------------------- */

void display_stats();

#endif /* STATS_H */