#include "threadpool.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


/*
    thread logic, waits for and executes tasks
*/
static void *thread_function(void *threadpool) {
    threadpool_t *pool = (threadpool_t *)threadpool;

    while(1) {
        pthread_mutex_lock(&(pool->lock));

        while (pool->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->shutdown && pool->count == 0) {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

       
        threadpool_task_t task = pool->queue[pool->head];

        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;

        // Free the lock before executing the task
        pthread_mutex_unlock(&(pool->lock));

        if (task.function != NULL) {
            (*(task.function))(task.arg_data);
        }
    }

    return NULL;
}

/*
    Allocatees memory and initializes a thread pool, if succesful returns a pointer to the newly created thread pool. NULL on failure.
*/
threadpool_t *threadpool_create(int thread_count, int queue_size) {
    if (thread_count <= 0 || queue_size <= 0) {
        return NULL;
    }

    threadpool_t *pool = (threadpool_t *)calloc(1, sizeof(threadpool_t));
    if (pool == NULL) {
        return NULL;
    }

    pool->queue_size = queue_size;

    pool->threads = (pthread_t *)calloc(thread_count, sizeof(pthread_t));
    pool->queue = (threadpool_task_t *)calloc(queue_size, sizeof(threadpool_task_t));

    if (pool->threads == NULL || pool->queue == NULL) {
        free(pool->threads);
        free(pool->queue);
        free(pool);
        return NULL;
    }

    if (pthread_mutex_init(&(pool->lock), NULL) != 0) {
        free(pool->threads);
        free(pool->queue);
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&(pool->notify), NULL) != 0) {
        pthread_mutex_destroy(&(pool->lock));
        free(pool->threads);
        free(pool->queue);
        free(pool);
        return NULL;
    }

    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&(pool->threads[i]), NULL, thread_function, (void*)pool) != 0) {
            threadpool_destroy(pool);
            return NULL;
        }
        pool->thread_count++;
        pool->started++;
    }

    return pool;
}
/*
    Adds a new task to the queue. Returns 0 if succesful, -1 otherwise.
*/
int threadpool_add(threadpool_t *pool, void (*function)(void *), void *arg, size_t arg_size) {
    if (pool == NULL || function == NULL || arg_size > TASK_DATA_SIZE) {
        return -1;
    }

    pthread_mutex_lock(&(pool->lock));

    if (pool->count == pool->queue_size || pool->shutdown) {
        pthread_mutex_unlock(&(pool->lock));
        return -1; 
    }

    pool->queue[pool->tail].function = function;
    
    if (arg != NULL && arg_size > 0) {
        memcpy(pool->queue[pool->tail].arg_data, arg, arg_size);
    }
    
    pool->tail = (pool->tail + 1) % pool->queue_size; 
    pool->count++;

    pthread_cond_signal(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}


/*
    Destroys the thread pool. Returns 0 if succesful, -1 otherwise.
*/
int threadpool_destroy(threadpool_t *pool){
    if (pool == NULL) {
        fprintf(stderr, "threadpool does not exist\n");
        return -1;
    }

    pthread_mutex_lock(&(pool->lock));
    
    if (pool->shutdown == true) {
        pthread_mutex_unlock(&(pool->lock));
        fprintf(stderr, "already shutting down the threadpool\n");
        return -1;   
    }
    pool ->shutdown = true;
    pthread_cond_broadcast(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
    free(pool->threads);
    free(pool->queue);
    free(pool);
    return 0;
}

