#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
int money = 400;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *func(void *arg)
{
    int i = *(int *)arg + 1;
    pthread_mutex_lock(&mutex);		//上锁
    while(money < 200)			//判断余额是否足够
    {
        printf("线程%d 余额不足\n",i);
        pthread_cond_wait(&cond,&mutex);//余额不足，就添加到条件变量中
    }
    money -= 200;			//余额足够，取200块钱
    printf("thread %d +200￥\n",i);
    pthread_mutex_unlock(&mutex);	//解锁走人
    pthread_exit(NULL);
}
int main(void)
{
    pthread_t tid[5];
    int i;
    for(i=0;i<5;i++)
    {
        pthread_create(&tid[i],NULL,func,(void *)&i);
        usleep(1000);
    }
    //第一次打钱
    sleep(5);
    pthread_mutex_lock(&mutex);	        //只要是访问临界资源,都加锁再访问,以防万一
    money += 400;			//打400块钱过去
    printf("money +400\n");
    pthread_mutex_unlock(&mutex);	//访问完临界资源记得解锁
    pthread_cond_broadcast(&cond);	//唤醒全部成员
	
    //第二次打钱
    sleep(5);
    pthread_mutex_lock(&mutex);         //只要是访问临界资源,都加锁再访问,以防万一
    money += 200;			//打200块钱过去
    printf("money +200\n");
    pthread_mutex_unlock(&mutex);	//访问完临界资源记得解锁
    pthread_cond_signal(&cond);		//随机唤醒一个
	
    for(i=0;i<5;i++)
    {
        pthread_join(tid[i],NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}