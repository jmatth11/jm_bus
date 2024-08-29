#include "../deps/array_template/array_template.h"

#include "thread_pool.h"

#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

generate_array_template(thread_job, struct thread_job)

struct thread_pool {
  pthread_mutex_t lock;
  thread_job_array threads;
};

struct thread_pool* thread_pool_create(size_t n) {
  struct thread_pool *pool = malloc(sizeof(struct thread_pool));
  if (pool == NULL) {
    fprintf(stderr, "malloc thread pool failed.\n");
    return NULL;
  }
  if (!init_thread_job_array(&pool->threads, n)) {
    fprintf(stderr, "initializing thread job array failed.\n");
    free(pool);
    return NULL;
  }
  if (pthread_mutex_init(&pool->lock, NULL) < 0) {
    fprintf(stderr, "pthread mutex init failed.\n");
    return NULL;
  }
  return pool;
}

bool thread_pool_start_job(struct thread_pool *pool, thread_func callback, void *ctx, size_t n) {
  bool found = false;
  for (int i = 0; i < pool->threads.len; ++i) {
    struct thread_job *job = &pool->threads.thread_job_data[i];
    if (atomic_compare_exchange_strong(&job->occupied, &found, true)) {
      found = true;
      write(job->pipe_fd[thread_job_pipe_write], ctx, n);
      break;
    }
  }
  if (!found) {
    pthread_mutex_lock(&pool->lock);
    struct thread_job local = {};
    atomic_store(&local.occupied, true);
    insert_thread_job_array(&pool->threads, local);
    struct thread_job *ref = &pool->threads.thread_job_data[pool->threads.len -1];
    pipe(ref->pipe_fd);
    pthread_create(&ref->thread_fd, NULL, callback, ref);
    pthread_mutex_unlock(&pool->lock);
  }
  return true;
}

void thread_pool_destroy(struct thread_pool *pool) {
  free_thread_job_array(&pool->threads);
  pthread_mutex_destroy(&pool->lock);
  free(pool);
}
