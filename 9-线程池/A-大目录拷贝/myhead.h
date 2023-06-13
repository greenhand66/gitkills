#ifndef _MYHEAD_H_
#define _MYHEAD_H_

#include"thread_pool.h"
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<signal.h>
#include"kernel_list.h"

#define BUFF_SIZE 1024

typedef struct filename{
    char src[4096];
    char dest[4096];
}filename;

typedef struct data{
    int id;
    char name[20];
    char pwd[15];
}data;

typedef struct node{
    data data;
    struct list_head list;
}node;

typedef struct msgtext{
    char type[10];
    int id;
    char name[20];
    char mtext[512];
}msgtext;

typedef struct msg_buf{
    long mtype;
    msgtext mtext; 
}msg_buf;

extern int msg_id;
extern unsigned long total_size;
extern unsigned long complete_size;
extern thread_pool *pool;
extern int temp;
extern node *online_usr;
extern pthread_mutex_t lock;

extern void quit_all(int signum);
extern node *kl_init();
extern void usr_show(node *head);
extern void kl_show(node *head);
extern void add_info_to_file(data file_data);
extern void read_file_to_link(node *head);
extern void empty_and_rewrite_info_to_file(node *head);
extern void empty_and_rewrite_info_to_ufile(node *head);
extern void read_file_to_ulink(node *head);
extern void fresh_now_usr(node *usr);
extern void on_or_un_line_send(node *head,node *usr,char *str);
extern void kl_add_tail(data new_data,node *head);
extern void kl_del(data del_data,node *head);
extern void destory_klist(node *head);
extern node *find_usr_from_ufile(node *usr);

extern void *recv(void *arg);
extern void update_info(node *head,node *usr);
extern node *login(node *head);
extern void panel(node *head,node *usr);
extern void private_chat(node *head,node *usr);
extern void send_msg(node *head,node *usr);
extern void print_frineds(node *usr);
extern void print_usr(node *head);
extern void group_chat(node *head,node *usr);
extern void usr_add(node *head);
extern node *run(node *head);
extern void friend_request(node *head,node *usr);
extern void send_request(node *head,node *usr);

extern void send_file(node *usr);
extern void send_file_private(node *usr);
extern void send_file_all(node *usr);
extern void fresh_friend_list(node *usr,int id,char *name);
extern node *find_friend_from_file(node *usr,int id);
extern node *find_node(node *head,data usr_data);
extern void empty_now_usr_file();
extern void cp(char *argv1,char *argv2);
extern void *file_copy(void *arg);
extern void *dir_copy(void *arg);
extern void get_size(char *src_dir,unsigned long *len);

#endif