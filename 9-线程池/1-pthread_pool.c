#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#include<string.h>
#include<strings.h>
#include<errno.h>

#define MAX_ACTIVE_THREADS 20
#define MAX_WAITING_TASKS    1000

struct task{
    void *(*do_task)(void *);
    void *arg;
    struct task *next;
};

// 线程池管理结构体
typedef struct thread_pool
{
//互斥锁（保证线程池的线程，在操作临界资源时，不会同时访问）
	pthread_mutex_t lock;
//条件变量（没有线程执行任务时，线程进入到睡眠状态，不去抢占资源）
	pthread_cond_t  cond;
//线程池是否关闭（销毁时使用）
	bool shutdown;
//线程任务链表
	struct task *task_list;
//线程号
	pthread_t *tids;
//最大等待任务数量
	unsigned max_waiting_tasks;
//任务列表中需要处理的任务数量
	unsigned waiting_tasks;
//活跃的线程数量（有多少线程在工作）
	unsigned active_threads;
}thread_pool;
//初始化线程池
bool init_pool(thread_pool *pool,unsigned int threads_number)
{
    //1.初始化线程池互斥锁
    pthread_mutex_init(&pool->lock,NULL);
    //2.初始化条件变量
    pthread_cond_init(&pool->cond,NULL);
    //3.初始化标志位为false，代表当前线程正在运行
    pool->shutdown = false;
    //4.初始化任务队列的头节点
    pool->task_list = malloc(sizeof(struct task));
    //5.为存储线程ID号申请空间
    pool->tids = malloc(sizeof(pthread_t)* MAX_ACTIVE_THREADS);
    if(pool->task_list == NULL || pool->tids == NULL)
    {
        perror("allocate memory error!");
        return false;
    }
    //6.为线程池任务队列的头节点的指针域赋值NULL
    pool->task_list->next = NULL;
    //7.设置线程池最大任务个数为1000
    pool->max_waiting_tasks = MAX_WAITING_TASKS;
    //8.当前需要处理的任务为0
    pool->waiting_tasks = 0;
    //9.初始化线程池中线程个数
    pool->active_threads = threads_number;

    //10.创建线程
    int i;
    for(i = 0;i < pool->active_threads;i++)
    {
        if(pthread_create(&((pool->tids)[i]),NULL,routine,(void*)pool) != 0)
        {
            perror("create threads error!");
            return false;
        }
    }

    //11.线程池初始化成功
    return true;
}

//向线程池中添加任务
bool add_task(thread_pool *pool,void *(*do_task)(void *arg),void *arg)
{
    //1.为新任务的节点申请空间
    struct task *new_task = malloc(sizeof(struct task));
    if(new_task == NULL)
    {
        perror("allocate memory error!");
        return false;
    }

    //2.为新新结点的数据域与指针赋值
    new_task->do_task = do_task;
    new_task->arg = arg;
    new_task->next = NULL;

    //3.在添加任务之前，必须先上锁，因为添加任务要访问临界资源（任务队列）
    pthread_mutex_lock(&pool->lock);

    //4.如果当前需要处理的任务个数>=1000，释放新添加的任务，然后退出
    if(pool->waiting_tasks >= MAX_WAITING_TASKS)
    {
        //解锁
        pthread_mutex_unlock(&pool->lock);
        fprintf(stderr,"too many tasks.\n");
        free(new_task);
        return false;
    }

    //5.如果任务数量未超过1000
    //将新的任务节点尾插至任务队列（链表）
    struct task *temp = pool->task_list;
    while(temp->next != NULL)
    {
        temp = temp->next;
    }

    //6.让最后一个节点的指针域指向新结点
    temp->next = new_task;
    //7.当前需要处理的任务+1
    pool->waiting_tasks++;
    //8.添加完毕，解锁
    pthread_mutex_unlock(&pool->lock);
    //9.唤醒条件变量中的一个线程做任务
    pthread_cond_signal(&pool->cond);
    return true;
}

//防死锁函数
void handler(void *arg)
{
    printf("[%u] is ended.\n",(unsigned)pthread_self());
    //解锁
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}

//线程执行函数
//一个线程去任务队列中拿出一个节点出来，执行该任务；若没有任务则阻塞等待任务
void *routine(void *arg)
{
    //1.先获取线程池的地址
    thread_pool *pool = (thread_pool*)arg;
    struct task *p;
    while(1)
    {
        //2.线程取消例程函数
        pthread_cleanup_push(handler,(void*)&pool->lock);

        //3.上锁
        pthread_mutex_lock(&pool->lock);

        //若当前线程没有关闭，且当前线程池没有任务做
        while(pool->waiting_tasks == 0 && pool->shutdown == false)
        {
            //到条件变量中挂起
            pthread_cond_wait(&pool->cond,&pool->lock);
        }
        //判断当前线程池是否关闭，且没有等待的任务
        if(pool->waiting_tasks == 0 && pool->shutdown == true)
        {
            //若线程池关闭，且无任务做
            //线程池解锁
            pthread_mutex_unlock(&pool->lock);
            //线程退出
            pthread_exit(NULL);
        }

        //当有任务做且线程池未关闭
        //就将头节点的下一个节点摘除任务队列，执行该节点的任务
        //p指向头节点的下一个
        p = pool->task_list->next;
        //让头节点的指针域指向p的下一个节点
        pool->task_list->next = p->next;
        //当前任务个数-1
        pool->waiting_tasks--;
        //解锁
        pthread_mutex_unlock(&pool->lock);

        //删除线程取消例程函数
        pthread_cleanup_pop(0);

        //设置线程不能响应取消
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);

        //执行这个p节点指向的节点的函数
        (p->do_task)(p->arg);

        //设置线程能响应取消
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

        //释放p对应的空间
        free(p);        
    }
    pthread_exit(NULL);
}

//向线程池中添加num个线程
int add_thread(thread_pool *pool,unsigned additional_threads)
{
    //若添加0条，则直接返回
    if(additional_threads == 0)
        return 0;

    unsigned total_threads = pool->active_threads + additional_threads;

    int i,actual_increment = 0;
    for(i = pool->active_threads;i < total_threads && i < MAX_ACTIVE_THREADS;i++)
    {
        if(pthread_create(&(pool->tids)[i],NULL,routine,(void*)pool) != 0)
        {
            perror("add threads error");
            if(actual_increment == 0)   //第一条创建失败
            {
                return -1;//直接返回
            }
            break;
        }
        actual_increment++; //真正创建线程的条数
    }
//当前线程池里的线程数 = 原来的个数 + 实际创建的个数
    pool->active_threads += actual_increment;
    return actual_increment;//返回真正创建的个数
}

//删除线程
int remove_thread(thread_pool *pool,unsigned int removing_threads)
{
    //删除0条，则直接返回
    if(removing_threads == 0)
        return pool->active_threads;
    
    //现线程池中剩余线程条数 = 原线程池中线程条数 减去 要删除的线程条数
    int remaining_threads = pool->active_threads - removing_threads;
    //若删除指定数量线程后，剩余线程数大于0，则剩余线程数不变，若小于0，则剩余线程数为1
    remaining_threads = remaining_threads > 0 ? remaining_threads : 1;
    int i,errno;
    for(i = pool->active_threads-1;i > remaining_threads;i--)
    {
        errno = pthread_cancel(pool->tids[i]);//取消线程
        if(errno != 0)
            break;
    }
    if(i == pool->active_threads-1)//删除失败
        return -1;
    else 
    {
        pool->active_threads = i+1;//当前实际线程数（for循环退出时，i减了1）
        return i+1;
    }
}

//销毁线程池
bool destroy_pool(thread_pool *pool)
{
    //设置线程池已关闭
    pool->shutdown = true;
    //唤醒所有挂起的线程
    pthread_cond_broadcast(&pool->cond);
    //接合所有线程
    int i,errno;
    for(i = 0;i < pool->active_threads;i++)
    {
        errno = pthread_join(pool->tids[i],NULL);
        if(errno != 0)
        {
            printf("join tids[%d] error: %s\n",i,strerror(errno));
        }
        else
            printf("[%u] is joined\n",(unsigned)pool->tids[i]);
    }
    //释放一些空间
    free(pool->task_list);
    free(pool->tids);
    free(pool);

    return true;
}