#include"myhead.h"

FILE *fp;

void fresh_now_usr(node *usr)
{
    fp = fopen("now_usr.txt","a");
    if(fp == NULL)
    {
        perror("fopen failed!\n");
        return;
    }

    int num = fprintf(fp,"%d %s ",usr->data.id,usr->data.name);
    if(num == -1)
    {
        printf("fwrite %s failed!\n","now_usr.txt");
        return;
    }

    fclose(fp);
    return;
}

node *find_usr_from_ufile(node *usr)
{
    fp = fopen("now_usr.txt","r");
    if(fp == NULL)
    {
        perror("fopen failed!\n");
        return NULL;
    }
    data new_data;
    int num;
    while(1)
    {
        bzero(&new_data,sizeof(data));
        num = fscanf(fp,"%d %s ",&new_data.id,new_data.name);
        if(num == EOF)
        {
            printf("fscanf done!\n");
            break;
        }
        if(new_data.id == usr->data.id)
            return usr;
    }
    fclose(fp);
    return NULL; 
}

void read_file_to_ulink(node *head)
{
    fp = fopen("now_usr.txt","r");
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
        num = fscanf(fp,"%d %s ",&new_data.id,new_data.name);
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

void empty_and_rewrite_info_to_ufile(node *head)
{
    if(list_empty(&head->list))
    {
        printf("用户数为0\n");
        return;
    }
    fp = fopen("now_usr.txt","w");
    if(fp == NULL)
    {
        printf("fopen failed!\n");
        return;
    }
    int num;
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        fprintf(fp,"%d %s ",pos->data.id,pos->data.name);
        printf("%d %s\n",pos->data.id,pos->data.name);
    }
    fclose(fp);
    return;
}

void empty_now_usr_file()
{
    fp = fopen("now_usr.txt","w");
    if(fp == NULL)
    {
        printf("fopen failed!\n");
        return;
    }
    fclose(fp);
    return;
}