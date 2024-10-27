#ifndef SET_SIGNALS_H
# define SET_SIGNALS_H

#include "typedefs.h"

void        ping(__attribute__((unused)) int signal);
void        stop(__attribute__((unused)) int signal);
FT_RESULT   set_signals(void);
FT_RESULT   reset_signals(void);

#endif /* SET_SIGNALS_H */