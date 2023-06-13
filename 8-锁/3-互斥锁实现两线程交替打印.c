#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

pthread_mutex_t lock;

void *func1(void *argv)
{
    pthread_mutex_lock(&lock);
    printf("pthread: %lu func: %s\n",pthread_self(),__FUNCTION__);
    for(int i = 1;i < 5;i++)
    {
        printf("%d\n",i);
        sleep(1);
    }
    pthread_mutex_unlock(&lock);
}

void *func2(void *argv)
{
    pthread_mutex_lock(&lock);
    printf("pthread: %lu func: %s\n",pthread_self(),__FUNCTION__);
    for(int i = 5;i < 9;i++)
    {
        printf("%d\n",i);
        sleep(1);
    }
    pthread_mutex_unlock(&lock);
}

int main(void)
{
    pthread_mutex_init(&lock,NULL);
    pthread_t tid1,tid2;
    pthread_create(&tid2,NULL,func2,NULL);
    pthread_create(&tid1,NULL,func1,NULL);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}