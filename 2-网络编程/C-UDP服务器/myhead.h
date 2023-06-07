#ifndef __MYHEAD_H_
#define __MYHEAD_H_

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>

typedef struct data{
    char ip[20];
    int port;
    char name[30];
}data;


//存放用户信息的节点模型
typedef struct node{
    data user_data;
    struct node *next;
}node;

//初始化链表的函数
extern node *init();
//尾插函数
extern void add_tail(node *head,data user_data);
//删除节点的函数
extern void del(node *head,data user_data);
//遍历链表的函数
extern void show(node *head);
extern void show_list_to_usr(int sockfd,node *head,node *usr);
extern node* find_node(node *head,data new_data);

extern int sockfd_init(char *port);

extern int detect_colon(char *str);
extern void send_to_someone(int sockfd,node *head,node *usr,char *buf);
extern void bcast_and_group_send(int sockfd,node *usr,char *buf,char *ip,int port);

#endif