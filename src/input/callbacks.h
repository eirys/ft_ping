#ifndef CALLBACKS_H
# define CALLBACKS_H

#include "typedefs.h"

FT_RESULT ttl_check(void* value, void* pflag);
FT_RESULT hex_check(void* value, void* pflag);
FT_RESULT linger_check(void* value, void* pflag);
FT_RESULT interval_check(void* value, void* pflag);

#endif /* CALLBACKS_H */