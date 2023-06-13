#include "threadpool.h"

bool init_pool(pthread_pool *pool,unsigned int pthread_number)
{
    pool->shutdown = false;
    pthread_mutex_init(&pool->lock,NULL);
    pthread_cond_init(&pool->cond,NULL);
    pool->waiting_tasks = 0;
    pool->tids = malloc(sizeof(pthread_t)*MAX_ACTIVE_THREADS);
    pool->task_list = malloc(sizeof(struct task)*MAX_WAITING_TASKS);
    if(pool->tids == NULL||pool->task_list == NULL)
    {
        perror("malloc tids and task_list faied!");
        return false;
    }
    pool->task_list->next = NULL;
    int i;
    for(i = 0;i < pthread_number;i++)
    {
        if(pthread_create(&pool->tids[i],NULL,routine,(void*)pool) != 0)
        {
            perror("pthread create failed!");
            return false;
        }
        pool->active_threads++;
    }
    return true;
}

void handler(void *arg)
{
    pthread_mutex_unlock((pthread_mutex_t*)arg);
}

void *routine(void *arg)
{
    pthread_pool *pool = (pthread_pool*)arg;
    struct task *p = NULL;
    while(1)
    {
        pthread_cleanup_push(handler,(void*)&pool->lock);
        pthread_mutex_lock(&pool->lock);
        while(pool->waiting_tasks == 0 && pool->shutdown == false)
        {
            pthread_cond_wait(&pool->cond,&pool->lock);
        }
        while(pool->waiting_tasks == 0 && pool->shutdown == true)
        {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }
        p = pool->task_list->next;
        pool->task_list->next = p->next;
        pool->waiting_tasks--;
        pthread_mutex_unlock(&pool->lock);
        pthread_cleanup_pop(0);                                             
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
        p->do_task(p->arg);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
        free(p);
    }
    pthread_exit(NULL);
}

bool add_task(pthread_pool *pool,void *(*do_task)(void *arg),void *arg)
{
    struct task *new = malloc(sizeof(struct task));
    if(new == NULL)
    {
        perror("malloc new failed!");
        return false;
    }
    new->do_task =  do_task;
    new->arg = arg;
    new->next = NULL;
    pthread_mutex_lock(&pool->lock);
    if(pool->waiting_tasks >= MAX_WAITING_TASKS)
    {
        perror("too many tasks!");
        free(new);
        return false;
    }
    struct task *temp;
    temp = pool->waiting_tasks;
    while(temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = new;
    pool->waiting_tasks++;
    pthread_mutex_unlock(&pool->lock);
    pthread_cond_signal(&pool->cond);
    return true;
}