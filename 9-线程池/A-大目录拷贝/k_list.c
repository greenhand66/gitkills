#include"myhead.h"

FILE *fp;

node *kl_init()
{
    node *head = malloc(sizeof(node));
    if(head == NULL)
    {
        printf("malloc head failed!\n");
        return NULL;
    }
    head->data.id = 0;
    bzero(head->data.name,sizeof(head->data.name));
    bzero(head->data.pwd,sizeof(head->data.pwd));
    
    INIT_LIST_HEAD(&head->list);
    return head;
}

void usr_show(node *head)
{
    printf("id\t pwd\t name\n");
    kl_show(head);
    return;
}

void kl_show(node *head)
{
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        printf("%-d\t %-s\t %-s\n",pos->data.id,pos->data.pwd,pos->data.name);
    }
    return;
}

void add_info_to_file(data file_data)
{
    fp = fopen("usr_info.txt","a");
    if(fp == NULL)
    {
        perror("fopen failed!\n");
        return;
    }

    int num = fprintf(fp,"%d %s %s ",file_data.id,file_data.pwd,file_data.name);
    // int num = fwrite(&file_data,sizeof(data),1,fp);
    if(num == -1)
    {
        perror("fwrite failed!\n");
        return;
    }

    fclose(fp);
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
            bzero(&pos->data,sizeof(data));
            list_del(&pos->list);
            free(pos);
            printf("删除成功!\n");
            if(head != online_usr)
                empty_and_rewrite_info_to_file(head);
            return;
        }
    }
    if(pos == head)
        printf("该用户不存在\n");
    return;
}

node *find_node(node *head,data usr_data)
{
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        if(pos->data.id == usr_data.id)
        {
            return pos;
        }
    }
    if(pos == head)
    {
        return NULL;
    }
}

void read_file_to_link(node *head)
{
    fp = fopen("usr_info.txt","r");
    if(fp == NULL)
    {
        perror("fopen failed!\n");
        return;
    }
    data new_data;
    int num;
    while(1)
    {
        bzero(&new_data,sizeof(data));
        num = fscanf(fp,"%d %s %s ",&new_data.id,new_data.pwd,new_data.name);
        if(num == EOF)
        {
            printf("fscanf done!\n");
            break;
        }
        kl_add_tail(new_data,head);
    }
    fclose(fp);
    return; 
}

void empty_and_rewrite_info_to_file(node *head)
{
    if(list_empty(&head->list))
    {
        printf("用户数为0\n");
        return;
    }
    fp = fopen("usr_info.txt","w");
    if(fp == NULL)
    {
        printf("fopen failed!\n");
        return;
    }
    int num;
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        fprintf(fp,"%d %s %s ",pos->data.id,pos->data.pwd,pos->data.name);
    }
    fclose(fp);
    return;
}

void destory_klist(node *head)
{
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        list_del(&pos->list);
        free(pos);
    }
    list_del(&pos->list);
    free(pos);
}