#ifndef _MYHEAD_H
#define _MYHEAD_H

#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include"kernel_list.h"

typedef struct data{
    int id;
    char name[20];
    int pwd;
    int tel;
}data;

typedef struct node{
    data data;
    struct list_head list;
}node;

FILE *fp;

extern void panel();
extern void regular_usr();
extern void login_usr();
//初始化链表
extern node *kl_init();
 //内核链表遍历
extern void kl_show(node *head);
//头插法创建新节点
extern void kl_add(data new_data,node *head);
//尾插法创建新节点
extern void kl_add_tail(data new_data,node *head);
//根据数据查找指定节点
extern node *find_node(node *head,data usr_data);
//内核链表删除节点
extern void kl_del(data del_data,node *head);
//添加用户
extern void usr_add(node *head);
 //删除用户
extern void usr_del(node *head);
 //显示用户信息
extern void usr_show(node *head);
//用户登录
extern node *login(node *head);
//用户信息修改
extern void update_info(node *head);
// 按块读取链表数据并存入文件
extern void add_info_to_file(data file_data);
//读文件到链表
extern void read_file_to_link(node *head);
//以清空的形式打开文件 将链表数据写入到文件
extern void empty_and_rewrite_info_to_file(node *head);

extern int show_all_dir(const char *src_dir,int deep);

extern int show_special_dir(const char *src_dir,int deep);

extern int copy_all_dir(const char *src_dir,const char *dest_dir);

extern void copy_file(const char *src_name,const char *dest_name);

extern int copy_special_dir(const char *src_dir,const char *dest_dir);

#endif
