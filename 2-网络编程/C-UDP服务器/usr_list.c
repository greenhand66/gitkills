#include"myhead.h"

//初始化链表的函数
node *init()
{
    node *head = malloc(sizeof(node));
    if(head == NULL)
    {
        perror("malloc head failed!");
        return NULL;
    }
    bzero(head->user_data.name,sizeof(head->user_data.name));    
    bzero(head->user_data.ip,sizeof(head->user_data.ip));    
    head->user_data.port = 0;
    head->next = NULL;
    return head;
}
//尾插函数
void add_tail(node *head,data user_data)
{   
    node *p;
    node *new = init();
    if(new == NULL)
        return;
    new->user_data = user_data;
    for(p = head;p->next != NULL;p = p->next);
    p->next = new;
    return;
}

node* find_node(node *head,data new_data)
{
    node *p;
    for(p = head->next;p != NULL;p = p->next)
    {
        if(strcmp(p->user_data.ip,new_data.ip) == 0 && p->user_data.port == new_data.port)
        {
            return p;
        }
    }
    return NULL;
}
//删除节点的函数
void del(node *head,data user_data)
{
    node *pos = find_node(head,user_data);
    if(pos ==NULL)
    {
        printf("该节点不存在！\n");
        return;
    }
    node *p;
    //找到要删除节点的前一个节点
    for(p = head;p->next != pos;p = p->next);
    p->next = pos->next;
    free(pos);
    return;
}
//遍历链表的函数
void show(node *head)
{
    node *p;
    for(p = head->next;p != NULL;p = p->next)
    {
        printf("IP:%s port:%d name:%s\n",p->user_data.ip,p->user_data.port,p->user_data.name);
    }
    return;
}

// void destory_list(node *head)
// {
//     node *p,*q;
//     for(p = head;p != NULL;p = p->next)
//     {

//     }
// }