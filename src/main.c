#include "ft_string.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv, char** env) {
    for (i32 i = 0; i < argc; i++) {
        String arg = createString(argv[i]);
        printf("Argument %d: %s\n", i, arg.m_data);
        destroyString(&arg);
    }

    (void)env;

    return 0;
}