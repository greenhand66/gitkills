#include"myhead.h"

void read_file_to_link(node *head)
{
    fp = fopen("../src/usr_info.txt","a+");
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
        num = fread(&new_data,sizeof(data),1,fp);
        if(num == 0)
        {
            // printf("fread done!\n");
            break;
        }
        kl_add_tail(new_data,head);
    }
    fclose(fp);
    return; 
}

void empty_and_rewrite_info_to_file(node *head)
{
    if(list_empty)
    {
        printf("用户数为0\n");
        return;
    }
    fp = fopen("../src/usr_info.txt","w");
    if(fp == NULL)
    {
        printf("fopen failed!\n");
        return;
    }
    int num;
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {

        fwrite(&pos->data,sizeof(data),1,fp);
    }
    fclose(fp);
    return;
}

void add_info_to_file(data file_data)
{
    fp = fopen("../src/usr_info.txt","a");
    if(fp == NULL)
    {
        perror("fopen failed!\n");
        return;
    }

    int num = fwrite(&file_data,sizeof(data),1,fp);
    if(num == 0)
    {
        perror("fwrite failed!\n");
        return;
    }

    fclose(fp);
    return;
}