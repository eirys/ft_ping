#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void log_info(const char* message, ...) {
    va_list args;
    va_start(args, message);

    vfprintf(stdout, message, args);
    fprintf(stdout, "\n");

    va_end(args);
}

void log_error(const char* message, ...) {
    va_list args;
    va_start(args, message);

    fprintf(stderr, "error: ");
    vfprintf(stderr, message, args);
    fprintf(stdout, "\n");

    va_end(args);
}

#ifdef __DEBUG
void log_debug(const char* function_name, const char* message, ...) {
    va_list args;
    va_start(args, message);

    fprintf(stdout, "[Debug from `%s'] ", function_name);
    vfprintf(stdout, message, args);
    fprintf(stdout, "\n");

    va_end(args);
}

void debug_msg(const void* payload) {
    u8 buf2[56];
    memcpy(buf2, payload, (56));

    for (int i = 0; i < (56); i++)
        printf("%02x ", buf2[i]);

    printf("\n");
}
void debug_packet(const void* packet) {
    u8 buf2[64];
    memcpy(buf2, packet, (64));

    for (int i = 0; i < (64); i++)
        printf("%02x ", buf2[i]);

    printf("\n");

}

#else
void log_debug(const char* function_name, const char* message, ...) { (void)function_name; (void)message; }
void debug_msg(const void* payload) { (void)0; }
void debug_packet(const void* packet) { (void)0; }
#endif
