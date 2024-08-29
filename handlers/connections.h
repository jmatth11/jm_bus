#ifndef JM_BUS_MESSAGES_HANDLER_H
#define JM_BUS_MESSAGES_HANDLER_H

#include <sys/cdefs.h>

__BEGIN_DECLS

void* accept_messages(void *ctx) __THROWNL __nonnull((1));
void* process_messages(void *ctx) __THROWNL __nonnull((1));

void* send_messages(void *ctx) __THROWNL __nonnull((1));

__END_DECLS

#endif
