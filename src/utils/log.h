#ifndef LOG_H
# define LOG_H

#include "typedefs.h"

void        log_info(const char* message, ...);
void        log_debug(const char* function_name, const char* message, ...);
void        log_error(const char* message, ...);
void        debug_msg(const void* payload);
void        debug_packet(const void* packet);

#endif /* LOG_H */