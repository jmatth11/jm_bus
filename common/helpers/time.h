#ifndef JM_BUS_TIME_H
#define JM_BUS_TIME_H

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Sleep for a given amount of milliseconds.
 *
 * @param[in] milliseconds The time to sleep.
 */
void sleep_ms(int milliseconds) __THROWNL;

__END_DECLS

#endif
