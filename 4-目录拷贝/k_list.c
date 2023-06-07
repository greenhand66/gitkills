#include"myhead.h"

node *kl_init()
{
    node *head = malloc(sizeof(node));
    if(head == NULL)
    {
        printf("malloc head failed!\n");
        return NULL;
    }

    bzero(&head->data,sizeof(data));
    INIT_LIST_HEAD(&head->list);
    return head;
}

void kl_show(node *head)
{
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        printf("%d\t %s\t %d\t %d\n",pos->data.id,pos->data.name,pos->data.pwd,pos->data.tel);
    }
    return;
}

void kl_add_tail(data new_data,node *head)
{
    node *new = kl_init();
    if(new == NULL)
        return;
    new->data = new_data;
    list_add_tail(&new->list,&head->list);
    return;
}

void kl_add(data new_data,node *head)
{
    node *new = kl_init();
    if(new == NULL)
        return;
    new->data = new_data;
    list_add(&new->list,&head->list);
    return;
}

node *find_node(node *head,data usr_data)
{
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        if(pos->data.id == usr_data.id)
            return pos;
    }
    if(pos == head)
    {
        return NULL;
    }
}

void kl_del(data del_data,node *head)
{
    if(list_empty(&head->list))
    {
        printf("用户数为0\n");
        return;
    }

    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        if(pos->data.id == del_data.id)
        {
            list_del(&pos->list);
            free(pos);
            printf("删除成功!\n");
            empty_and_rewrite_info_to_file(head);
            return;
        }
    }
    if(pos == head)
        printf("该用户不存在\n");
    return;
}
