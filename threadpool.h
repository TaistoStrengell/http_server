#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <stdbool.h>
#include <pthread.h>

typedef struct {
    void (*function)(void *);
    void *argument;
} threadpool_task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    threadpool_task_t *queue;
    int thread_count;
    int queue_size;
    int head;
    int tail;
    int count;
    bool shutdown;
    int started;
} threadpool_t;

threadpool_t *threadpool_create(int thread_count, int queue_size);
int threadpool_add(threadpool_t *pool, void (*function)(void *), void *argument);
int threadpool_destroy(threadpool_t *pool);

#endif