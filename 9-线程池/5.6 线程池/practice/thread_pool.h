#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<error.h>
#include<pthread.h>

//最大等待任务数
#define MAX_WAITING_TASKS   1000
//最大的活跃线程数
#define MAX_ACTIVE_THREADS  20

struct task
{
    void *(*task)(void *arg);
//任务（函数指针指向任务函数，任务函数的返回值为void*,形参为void*）
    void *arg;
    struct task *next;
};

typedef struct thread_pool
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    struct task *task_list;//任务队列（结构体数组首地址）
    pthread_t *tids;//线程池线程tid数组首地址
    unsigned waiting_tasks;
    unsigned active_threads;
    bool shutdown;//是否关闭线程池
}thread_pool;


bool init_pool(thread_pool *pool,unsigned int threads_number);

//往线程池增加任务
bool add_task(thread_pool *pool,
                void *(*task)(void *arg),
                void *arg);

//往线程池新增线程
int add_thread(thread_pool *pool,
                unsigned int additional_threads_number);

//从线程池移除线程
int remove_thread(thread_pool *pool,unsigned int removing_threads_number);

//销毁线程池
bool destroy_pool(thread_pool *pool);

//任务
void *routine(void *arg);

#endif