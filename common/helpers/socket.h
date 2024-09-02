#ifndef JM_BUS_SOCKEY_HELPERS_H
#define JM_BUS_SOCKEY_HELPERS_H

#include "types/array_types.h"
#include "types/message.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct sockaddr_in create_default_addr() __THROWNL;
struct sockaddr_in create_addr(sa_family_t __type, uint16_t port, int32_t addr) __THROWNL;
bool prepare_and_send_message(int socket, struct message *msg) __THROWNL __nonnull((2));
bool send_message(int socket, byte_array buf) __THROWNL;

__END_DECLS

#endif
