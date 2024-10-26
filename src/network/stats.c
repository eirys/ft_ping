#include "stats.h"

#include <math.h>

#include "raw_socket.h"
#include "log.h"

/* -------------------------------------------------------------------------- */

PingStats g_stats = {
    .m_start.tv_sec = 0,
    .m_start.tv_usec = 0,
    .m_min_rtt = 0.0,
    .m_max_rtt = 0.0,
    .m_total_rtt = 0.0,
    .m_total_rtt_square = 0.0,
    .m_packet_sent = 0U,
    .m_packet_received = 0U
};

/* -------------------------------------------------------------------------- */

void display_stats() {
    log_info("--- %s statistics ---", g_socket.m_ipv4_str);

    log_info("%d packets transmitted, %d packets received, %d%% packets lost",
    g_stats.m_packet_sent,
    g_stats.m_packet_received,
    (g_stats.m_packet_sent - g_stats.m_packet_received) * 100U);

    const double mean = g_stats.m_total_rtt / (double)g_stats.m_packet_sent;
    const double smean = g_stats.m_total_rtt_square / (double)g_stats.m_packet_sent;
    log_info("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms",
        g_stats.m_min_rtt,
        mean,
        g_stats.m_max_rtt,
        sqrt(smean - (mean * mean)) /* standard deviation */
    );
}