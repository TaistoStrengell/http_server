#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <stdbool.h>
#include <pthread.h>
#include <stddef.h>

#define TASK_DATA_SIZE 64

typedef struct {
    void (*function)(void *);
    char arg_data[TASK_DATA_SIZE];
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
int threadpool_destroy(threadpool_t *pool);
int threadpool_add(threadpool_t *pool, void (*function)(void *), void *arg, size_t arg_size);
#endif