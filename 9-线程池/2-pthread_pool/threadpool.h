#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<error.h>
#include<unistd.h>
#include<stdbool.h>

#define MAX_WAITING_TASKS 1000
#define MAX_ACTIVE_THREADS  20

struct task{
    void *(*do_task)(void*);
    void *arg;
    struct task *next;
};

typedef struct pthread_pool{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    struct task *task_list;
    pthread_t *tids;
    unsigned int waiting_tasks;
    unsigned int active_threads;
    bool shutdown;
}pthread_pool;

#endif
