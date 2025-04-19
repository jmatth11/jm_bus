#ifndef JM_BUS_THREAD_POOL_H
#define JM_BUS_THREAD_POOL_H

#include "types/message.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Callback function type to be used in a newly spawned thread.
 */
typedef void*(*thread_func)(void*);

/**
 * A thread job structure.
 * Used as the object to manage and share data between the main thread and the
 * spawned thread.
 */
struct thread_job {
  struct message_event event;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_t thread_fd;
  atomic_bool occupied;
};

/**
 * The thread pool to manage spawning and reusing threads.
 */
struct thread_pool;

/**
 * Create a thread pool with an initial amount of threads.
 *
 * @param[in] n The initial size of the thread pool.
 * @return A newly allocated thread pool, NULL for error.
 */
struct thread_pool* thread_pool_create(size_t n) __THROWNL;

/**
 * Start a job using the given thread_func and a message_event.
 *
 * @param[in] pool The thread pool.
 * @param[in] callback The thread_func.
 * @param[in] msg The message_event to use.
 * @return True for success, false otherwise.
 */
bool thread_pool_start_job(struct thread_pool*, thread_func, struct message_event) __THROWNL __nonnull((1,2));

/**
 * Destroy the thread pool and close all open threads.
 *
 * @param[out] pool The thread pool.
 */
void thread_pool_destroy(struct thread_pool**) __THROWNL __nonnull((1));

__END_DECLS

#endif
