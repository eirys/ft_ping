#ifndef CALLBACKS_H
# define CALLBACKS_H

#include "typedefs.h"

FT_RESULT uchar_check(char* value, void* pflag);
FT_RESULT hex_check(char* value, void* pflag);
FT_RESULT int_check(char* value, void* pflag);

#endif /* CALLBACKS_H */