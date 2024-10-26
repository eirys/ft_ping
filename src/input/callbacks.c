#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "typedefs.h"
#include "options.h"
#include "log.h"

/**
 * @brief Check if the value is a u8 number, then set flag to integer conversion.
 */
FT_RESULT ttl_check(void* value, void* pflag) {
    const char* copy = value;

    while (*copy != '\0') {
        if (!isdigit(*copy)) {
            log_error("invalid argument: `%s`", (char*)value);
            return FT_FAILURE;
        }
        ++copy;
    }

    const int result = atoi(value);
    if (result < 0 || result > UINT8_MAX) {
        log_error("failed to set ttl flag (value is outside of [0 - 255])");
        return FT_FAILURE;
    }

    *(u8*)pflag = (u8)result;

    return FT_SUCCESS;
}

/**
 * @brief Check if the value is hex, then set flag to integer conversion.
 */
FT_RESULT hex_check(void* value, void* pflag) {
    const char* copy = value;
    u32         len = 0;

    while (*copy) {
        if (!isxdigit(*copy)) {
            log_error("invalid argument: `%s`", (char*)value);
            return FT_FAILURE;
        }
        ++copy;

        if (++len > 16) {
            log_error("failed to set pattern flag (value too long, only up to 16 bytes allowed)");
            return FT_FAILURE;
        }
    }
    /* Process 2 bytes at a time */
    u64 result = 0;
    for (u32 i = 0; i < len; i += 2) {
        result <<= 8;
        u16 byte[2];
        memcpy(byte, value + i, 2);
        result |= strtol((const char*)byte, NULL, 16);
    }


    Pattern* pattern = (Pattern*)pflag;
    pattern->content = result;
    pattern->length = (len + 1) / 2;

    return FT_SUCCESS;
}

//TODO
FT_RESULT linger_check(void* value, void* pflag) {
    return 1;
}

FT_RESULT interval_check(void* value, void* pflag) {
    return 1;
}
