#include<stdio.h>
#include<pthread.h>

static pthread_rwlock_t rwlock;

int global = 0;

void *routine1(void *arg)
{
    //对共享资源进行写操作请之前，必须加写锁（互斥）
    pthread_rwlock_wrlock(&rwlock);

    global += 1;
    printf("I am %s,now global = %d\n",(char*)arg,global);

    //访问完之后释放该锁
    pthread_rwlock_unlock(&rwlock);

    pthread_exit(NULL);
}

void *routine2(void *arg)
{
    //对共享资源进行写操作请之前，必须加写锁（互斥锁）
    pthread_rwlock_wrlock(&rwlock);//writelock

    global = 100;
    printf("I am %s,now global = %d\n",(char*)arg,global);

    //访问完之后释放该锁
    pthread_rwlock_unlock(&rwlock);

    pthread_exit(NULL);
}

void *routine3(void *arg)
{
    //对共享资源进行读操作前，可以加读锁（共享锁）
    //写锁会导致读锁阻塞
    pthread_rwlock_rdlock(&rwlock);//readlock

    printf("I am %s,now global = %d\n",(char*)arg,global);

    //访问完之后释放该锁
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

int main()
{
    pthread_rwlock_init(&rwlock,NULL);

    //创建三条线程，对共享资源同时进行读写操作
    pthread_t t1,t2,t3;
    pthread_create(&t1,NULL,routine1,"thread 1");
    pthread_create(&t2,NULL,routine2,"thread 2");
    pthread_create(&t3,NULL,routine3,"thread 3");
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);

    //销毁读写锁
    pthread_rwlock_destroy(&rwlock);

    return 0;

}