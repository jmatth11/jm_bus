#ifndef JM_BUS_THREAD_POOL_H
#define JM_BUS_THREAD_POOL_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef void*(*thread_func)(void*);

enum thread_job_pipe_enum {
  thread_job_pipe_read = 0,
  thread_job_pipe_write = 1,
};

struct thread_job {
  int pipe_fd[2];
  pthread_t thread_fd;
  atomic_bool occupied;
};

struct thread_pool;

struct thread_pool* thread_pool_create(size_t n) __THROWNL;
bool thread_pool_start_job(struct thread_pool*, thread_func, void *, size_t n) __THROWNL __nonnull((1,2));
void thread_pool_destroy(struct thread_pool*) __THROWNL __nonnull((1));

__END_DECLS

#endif
