#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
/*
添加任务函数到链表，并用线程去执行任务
*/
typedef struct task{
    void* (*task)(void*);
    void* argv;
    struct task* next;
}task;

struct arg{
    int a;
    int b;
};

void* add(void* argv)
{
    struct arg *p = (struct arg*)argv;
    int *ret = malloc(4);
    *ret = p->a + p->b;
    printf("a+b = %d\n",*ret);
    pthread_exit(NULL);
}

void* show_bmp(void *bmppath)
{
    char *bmp_path = (char*)bmppath;
    printf("%s\n",bmp_path);
    pthread_exit(NULL);
}

int main(void)
{
    task *head = malloc(sizeof(task));
    if(head == NULL)
    {
        perror("malloc head failed!");
        return -1;
    }
    struct arg *data = malloc(sizeof(struct arg));
    data->b = 20;
    data->a = 10;
    task *new1 = malloc(sizeof(task));

    new1->argv = (void*)data;
    new1->task = add;
    head->next = new1;

    task *new2 = malloc(sizeof(task));
    new1->next = new2;
    new2->task = show_bmp;
    new2->argv = (void*)"1.bmp";

    pthread_t tid;
    for(task *p = head->next;p != NULL;p = p->next)
    {
        pthread_create(&tid,NULL,p->task,p->argv);//按照栈逻辑,new2先执行完毕，
    }
    sleep(1);
    pthread_join(tid,NULL);//只能回收new2那个线程，又因为new2先执行完毕，故要在前面延时，等待new1执行完毕
}