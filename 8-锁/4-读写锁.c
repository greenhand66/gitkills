#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<strings.h>

char buff[256];

pthread_rwlock_t rwlock;

void *reader1(void *argv)
{
    pthread_rwlock_rdlock(&rwlock);
    printf("reader1 :%s\n",buff);
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

void *reader2(void *argv)
{
    pthread_rwlock_rdlock(&rwlock);
    printf("reader2 :%s\n",buff);
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

void *writer1(void *argv)
{
    pthread_rwlock_wrlock(&rwlock);
    printf("writer1\n");
    fgets(buff,sizeof(buff),stdin);strtok(buff,"\n");
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

void *writer2(void *argv)
{
    pthread_rwlock_wrlock(&rwlock);
    printf("writer2\n");
    fgets(buff,sizeof(buff),stdin);strtok(buff,"\n");
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_rwlock_init(&rwlock,NULL);

    pthread_t rtid1,rtid2,wtid1,wtid2;
    pthread_create(&rtid1,NULL,reader1,NULL);
    pthread_create(&rtid2,NULL,reader2,NULL);
    pthread_create(&wtid1,NULL,writer1,NULL);
    pthread_create(&wtid2,NULL,writer2,NULL);

    pthread_join(rtid1,NULL);
    pthread_join(rtid2,NULL);
    pthread_join(wtid1,NULL);
    pthread_join(wtid2,NULL);

    pthread_rwlock_destroy(&rwlock);
    return 0;
}