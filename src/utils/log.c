#include "log.h"

#include <stdio.h>
#include <stdarg.h>

/* ========================================================================== */
/*                                     LOG                                    */
/* ========================================================================== */

void log_info(const char* message, ...) {
    va_list args;
    va_start(args, message);

    fprintf(stdout, "Info: ");
    vfprintf(stdout, message, args);

    va_end(args);
}

void log_error(const char* function_name, const char* message) {
    fprintf(stderr, "Error: [%s] %s\n", function_name, message);
}

void log_debug(const char* function_name, const char* message) {
    fprintf(stdout, "Debug: [%s] %s\n", function_name, message);
}