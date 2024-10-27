#include "stats.h"

#include <math.h>
#include <float.h>

#include "options.h"
#include "raw_socket.h"
#include "log.h"

/* --------------------------------- GLOBALS -------------------------------- */

bool g_ongoing = true;

/* -------------------------------------------------------------------------- */

PingStats g_stats = {
    .m_start.tv_sec = 0,
    .m_start.tv_usec = 0,
    .m_min_rtt = DBL_MAX,
    .m_max_rtt = 0.0,
    .m_total_rtt = 0.0,
    .m_total_rtt_square = 0.0,
    .m_packet_sent = 0U,
    .m_packet_received = 0U
};

/* -------------------------------------------------------------------------- */

void display_stats() {
    log_info("--- %s statistics ---", g_arguments.m_destination);

    log_info("%u packets transmitted, %u packets received, %u%% packets lost",
    g_stats.m_packet_sent,
    g_stats.m_packet_received,
    ((g_stats.m_packet_sent - g_stats.m_packet_received) / g_stats.m_packet_sent) * 100U);

    if (g_stats.m_packet_received == 0) {
        return;
    }
    const double mean = g_stats.m_total_rtt / (double)g_stats.m_packet_sent;
    const double smean = g_stats.m_total_rtt_square / (double)g_stats.m_packet_sent;
    log_info("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms",
        g_stats.m_min_rtt,
        mean,
        g_stats.m_max_rtt,
        sqrt(smean - (mean * mean)) /* standard deviation */
    );
}