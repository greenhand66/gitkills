#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

pthread_mutex_t m;

void output(char *string)
{
    const char *p = string;
    while(*p != '\0')
    {
        printf("%c",*p);
        usleep(100);
        p++;
    }
}

void *routine(void *argv)
{
    pthread_mutex_lock(&m);
    output("hello world!\n");
    pthread_mutex_unlock(&m);
    pthread_exit("byebye!\n");
}

int main()
{
    pthread_mutex_init(&m,NULL);

    pthread_t tid;
    pthread_create(&tid,NULL,routine,NULL);

    pthread_mutex_lock(&m);
    output("info ftom parent!\n");
    pthread_mutex_unlock(&m);

    char *p;
    pthread_join(tid,(void*)&p);
    printf("son return: %s\n",p);

    pthread_mutex_destroy(&m);

    return 0;
}
