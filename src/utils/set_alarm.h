#ifndef SET_ALARM_H
# define SET_ALARM_H

#include "typedefs.h"

void        ping(__attribute__((unused)) int signal);
FT_RESULT   set_alarm(void);
FT_RESULT   reset_alarm(void);


#endif /* SET_ALARM_H */