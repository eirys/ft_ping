#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "typedefs.h"
#include "options.h"
#include "log.h"

static
FT_RESULT _check_str_kind(char* value, int (*kind)(int)) {
    const char* copy = value;

    while (*copy != '\0') {
        if (kind(*copy) == 0) {
            log_error("invalid argument: `%s`", (char*)value);
            return FT_FAILURE;
        }
        copy++;
    }

    return FT_SUCCESS;
}

/**
 * @brief Check if the value is a u8 number, then set flag to integer conversion.
 */
FT_RESULT uchar_check(char* value, void* pflag) {
    if (_check_str_kind(value, isdigit) == FT_FAILURE) {
        return FT_FAILURE;
    }

    const int result = atoi(value);
    if (result < 0 || result > UINT8_MAX) {
        log_error("bad value for ttl, must be in range [0, 255]");
        return FT_FAILURE;
    }

    *(u8*)pflag = (u8)result;

    return FT_SUCCESS;
}

/**
 * @brief Check if the value is hex, then set flag to uint conversion.
 */
FT_RESULT hex_check(char* value, void* pflag) {
    u32         len = strlen(value);

    if (_check_str_kind(value, isxdigit) == FT_FAILURE) {
        return FT_FAILURE;
    }

    if (len > 16) {
        log_error("failed to set pattern flag (value too long, only up to 16 bytes allowed)");
        return FT_FAILURE;

    }

    // while (value[len]) {
    //     if (!isxdigit(value[len])) {
    //         log_error("invalid argument: `%s`", (char*)value);
    //         return FT_FAILURE;
    //     }
    //     if (++len > 16) {
    //         log_error("failed to set pattern flag (value too long, only up to 16 bytes allowed)");
    //         return FT_FAILURE;
    //     }
    // }

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

/**
 * @brief Check if value is integer, then set flag to integer conversion.
 */
FT_RESULT int_check(char* value, void* pflag) {
    if (_check_str_kind(value, isdigit) == FT_FAILURE) {
        return FT_FAILURE;
    }

    const long int result = atoi(value);
    if (result <= 0 || result > INT32_MAX) {
        log_error("bad value: `%s'", value);
        return FT_FAILURE;
    }

    *(int*)pflag = (int)result;

    return FT_SUCCESS;
}