#include "thread_pool.h"

void handler(void *arg)
{
    //互斥锁解锁
    pthread_mutex_unlock((pthread_mutex_t*)arg);
}

void *routine(void *arg)
{
        thread_pool *pool = (thread_pool *)arg;
        struct task *p;

        while(1)
        {
            pthread_cleanup_push(handler,(void*)&pool->lock);
            pthread_mutex_lock(&pool->lock);
            //当等待的任务为0且线程池未关闭
            while(pool->waiting_tasks == 0 && !pool->shutdown)
            {
                //条件变量等待任务到来
                pthread_cond_wait(&pool->cond,&pool->lock);
            }
            //当等待的任务为0且线程池关闭
            if(pool->waiting_tasks == 0 && pool->shutdown == true)
            {
                pthread_mutex_unlock(&pool->lock);
                pthread_exit(NULL);
            }

            p = pool->task_list->next;

        }
}