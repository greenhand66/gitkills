#include"myhead.h"

unsigned long total_size;
unsigned long complete_size;
thread_pool *pool;
int msg_id;
pthread_mutex_t lock;
node *online_usr;
int temp;

int main(int argc,char **argv)
{
    if(argc == 3)
    {
        cp(argv[1],argv[2]);
        return 0;
    }
    
    node *head = kl_init();
    if(head == NULL)
        return -1;
    read_file_to_link(head); 
    signal(2,quit_all);
    pthread_mutex_init(&lock,NULL);
    run(head);
    pthread_mutex_destroy(&lock);
    destory_klist(head);  
    return 0;
}

void quit_all(int signum)
{
    printf("quit\n");
    if(list_empty(&online_usr->list))
    {
        printf("用户数为0，退出程序\n");
        int ret = msgctl(msg_id,IPC_RMID,NULL);
        if(ret != 0)
            perror("panel msgrm failed!");
    }
    // destory_klist(head);
    exit(0);
}