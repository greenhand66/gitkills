#include "threadpool.h"

void *func(void *arg)
{
    char *str = (char*)arg;
    printf("%s\n",str);
    return  NULL;
}

int main(void)
{
    pthread_pool *pool = malloc(sizeof(pthread_pool));
    init_pool(pool,3);
    int i;
    char str[20];
    for(i = 1;i <= 4;i++)
    {
        sprintf(str,"当前是线程%d",i);
        add_task(pool,func);
        usleep(1000);
    }
    sleep(20);
    return 0;
}