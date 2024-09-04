#include "../deps/array_template/array_template.h"

#include "thread_pool.h"

#include "helpers/log.h"

#include <pthread.h>
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
    error_log("malloc thread pool failed.\n");
    return NULL;
  }
  if (!init_thread_job_array(&pool->threads, n)) {
    error_log("initializing thread job array failed.\n");
    free(pool);
    return NULL;
  }
  if (pthread_mutex_init(&pool->lock, NULL) < 0) {
    error_log("pthread mutex init failed.\n");
    return NULL;
  }
  return pool;
}

bool thread_pool_start_job(struct thread_pool *pool, thread_func callback, struct message_event event) {
  bool found = false;
  for (int i = 0; i < pool->threads.len; ++i) {
    struct thread_job *job = &pool->threads.thread_job_data[i];
    if (atomic_compare_exchange_strong(&job->occupied, &found, true)) {
      found = true;
      job->event = event;
      // signal for the job to start
      pthread_cond_signal(&job->cond);
      break;
    }
  }
  if (!found) {
    pthread_mutex_lock(&pool->lock);
    struct thread_job local = {};
    atomic_store(&local.occupied, true);
    insert_thread_job_array(&pool->threads, local);
    struct thread_job *ref = &pool->threads.thread_job_data[pool->threads.len -1];
    ref->event = event;
    if (pthread_cond_init(&ref->cond, NULL) != 0) {
      error_log("pthread condition init failed.\n");
      pthread_mutex_unlock(&pool->lock);
      return false;
    }
    if (pthread_mutex_init(&ref->mutex, NULL) != 0) {
      error_log("pthread mutex init failed.\n");
      pthread_mutex_unlock(&pool->lock);
      return false;
    }
    if (pthread_create(&ref->thread_fd, NULL, callback, ref) != 0) {
      error_log("pthread create failed.\n");
      pthread_mutex_unlock(&pool->lock);
      return false;
    }
    // wait for thread to start up.
    pthread_mutex_lock(&ref->mutex);
    pthread_cond_wait(&ref->cond, &ref->mutex);
    pthread_mutex_unlock(&ref->mutex);

    pthread_mutex_unlock(&pool->lock);
    // signal to the thread to unblock
    pthread_cond_signal(&ref->cond);
  }
  return true;
}

void thread_pool_destroy(struct thread_pool *pool) {
  pthread_mutex_lock(&pool->lock);
  for (int i = 0; i < pool->threads.len; ++i) {
    struct thread_job *job = &pool->threads.thread_job_data[i];
    job->event.from = -1;
    pthread_cond_signal(&job->cond);
    pthread_join(job->thread_fd, NULL);
    pthread_cond_destroy(&job->cond);
    pthread_mutex_destroy(&job->mutex);
  }
  free_thread_job_array(&pool->threads);
  pthread_mutex_unlock(&pool->lock);
  pthread_mutex_destroy(&pool->lock);
  free(pool);
}
