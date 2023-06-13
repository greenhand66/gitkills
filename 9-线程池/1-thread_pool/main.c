#include "thread_pool.h"

void *mytask(void *arg)
{
	int n = (int)arg;

	printf("[%u][%s] ==> job will be done in %d sec...\n",
		(unsigned)pthread_self(), __FUNCTION__, n);

	sleep(n);

	printf("[%u][%s] ==> job done!\n",
		(unsigned)pthread_self(), __FUNCTION__);

	return NULL;
}

void *count_time(void *arg)
{
	int i = 0;
	while(1)
	{
		sleep(1);
		printf("sec: %d\n", ++i);
	}
}

void *count_and_print(void *arg)
{
    int n;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        for(n = 0;n < strlen(p);n++)
        {
            fprintf(stderr,"%c",p[n]);
        }
        printf("字符串: %s 长度为：%d\n",p,n);
        pthread_mutex_unlock(&mutex);
        if(!strncmp(p,"quit",4))
            break;
    }    
    shmdt(p);
    shmctl(shm_id,IPC_RMID,NULL);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
}

void *get_string(void *arg)
{
    shm_id = shmget(ftok(".",1),SHM_SIZE,IPC_CREAT|0666);
    if(shm_id == -1)
    {
        perror("shmget failed!");
        pthread_exit(NULL);
    }
    p = shmat(shm_id,NULL,0);
    if(p == (void*)-1)
    {
        perror("shmat failed!");
        pthread_exit(NULL);
    }
    while(1)
    {
        pthread_mutex_lock(&mutex);
        fgets(p,SHM_SIZE,stdin);strtok(p,"\n");
        pthread_mutex_unlock(&mutex);
        if(!strncmp(p,"quit",4))
            break;
    }
    pthread_exit(NULL);
}

int main(void)
{
	pthread_mutex_init(&mutex,NULL);

}
