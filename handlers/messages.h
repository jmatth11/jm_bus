#ifndef JM_BUS_MESSAGES_HANLDER_H
#define JM_BUS_MESSAGES_HANLDER_H

#include <stdbool.h>
#include "types/message.h"
#include <sys/cdefs.h>

__BEGIN_DECLS

bool messages_read(const byte_array msg, struct message *out) __THROWNL;
bool messages_write(const struct message *msg, byte_array *out) __THROWNL __nonnull((1));

__END_DECLS

#endif
