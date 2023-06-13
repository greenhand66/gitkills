#include"myhead.h"


void send_msg(node *head,node *usr)
{
    msg_buf mydata;
    data new_data;
    node *p;
    int flag1= 0;
    bzero(&new_data,sizeof(new_data));
    printf("请输入要私聊的好友ID\n");
    scanf("%d",&new_data.id);while(getchar()!='\n');
    p = find_node(online_usr,new_data);
    if(p == NULL)
    {
        perror("该用户不在线！");
        return;
    }
    mydata.mtype = p->data.id;
    while(1)
    {
        bzero(&mydata.mtext,sizeof(mydata.mtext));
printf("type  = %ld\n",mydata.mtype);
        mydata.mtext.id = usr->data.id;
        strncpy(mydata.mtext.type,"friend",6);
        strncpy(mydata.mtext.name,usr->data.name,(int)strlen(usr->data.name));
        printf("请输入消息（输入quit退出）\n");
        if(flag1 == 0)
        {
            pthread_mutex_lock(&lock);
            flag1 = 1;
        }
        fgets(mydata.mtext.mtext,sizeof(mydata.mtext.mtext),stdin);strtok(mydata.mtext.mtext,"\n");
        if(!strncmp(mydata.mtext.mtext,"quit",4))
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        if(atoi(mydata.mtext.mtext) == 111)
        {
            temp = 111;
            pthread_mutex_unlock(&lock);
            while(temp != 0);
            flag1 = 0;
        }
        else if(atoi(mydata.mtext.mtext) == 222)
        {
            temp = 222;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        else if(atoi(mydata.mtext.mtext) == 666)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        else if(atoi(mydata.mtext.mtext) == 888)
        {
            temp = 888;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        msgsnd(msg_id,&mydata,sizeof(mydata.mtext),0);
    }
    return;
}

void fresh_friend_list(node *usr,int id,char *name)
{
    char file_path[30];
    data buf;
    bzero(&buf,sizeof(buf));
    buf.id = id;
    strncpy(buf.name,name,strlen(name));
    bzero(file_path,sizeof(file_path));
    sprintf(file_path,"%d.txt",usr->data.id);
    printf("file_path %s\n",file_path);
    FILE *fp = fopen(file_path,"a+");
    if(fp == NULL)
    {
        printf("open %s failed!\n",file_path);
        return;
    }
    fprintf(fp,"%d %s ",buf.id,buf.name);
    fclose(fp);
}

node *find_friend_from_file(node *usr,int id)
{
    char file_path[30];
    bzero(file_path,sizeof(file_path));
    sprintf(file_path,"%d.txt",usr->data.id);
    FILE *fp = fopen(file_path,"r");
    if(fp == NULL)
    {
        printf("open %s failed!\n",file_path);
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
        if(new_data.id == id)
            return usr;
    }
    fclose(fp);
    return NULL;
}

void print_frineds(node *usr)
{
    char file_path[30];
    bzero(file_path,sizeof(file_path));
    sprintf(file_path,"%d.txt",usr->data.id);
    FILE *fp = fopen(file_path,"r");
    if(fp == NULL)
    {
        fp = fopen(file_path,"w+");
        if(fp == NULL)
        {
            perror("fopen failed!");
            return;
        }
    }
    printf("我的好友\n");
    data friend;
    int ret;
    printf("ID\t name\n");
    while(1)
    {
        bzero(&friend,sizeof(data));
        ret = fscanf(fp,"%d %s ",&friend.id,friend.name);
        if(ret <= 0)
            break;
        printf("%d\t %s\n",friend.id,friend.name);
    }
    fclose(fp);
    return;
}

void on_or_un_line_send(node *head,node *usr,char *str)
{
    msg_buf mydata;
    node *pos,*n,*t;
    bzero(&mydata,sizeof(mydata));
    mydata.mtext.id = usr->data.id;
    strncpy(mydata.mtext.name,usr->data.name,(int)strlen(usr->data.name));
    strncpy(mydata.mtext.type,str,strlen(str));
    list_for_each_entry_safe(pos,n,&head->list,list)//给所有用户的服务器发送
    {
        mydata.mtype = pos->data.id;
        t = find_usr_from_ufile(pos);
        if(t != NULL)
        {
            msgsnd(msg_id,&mydata,sizeof(mydata.mtext),0);
        }
    }
}

void print_usr(node *head)
{
    printf("ID\t name\n");
    node *pos,*n;
    list_for_each_entry_safe(pos,n,&head->list,list)
    {
        printf("%-d\t %-s\n",pos->data.id,pos->data.name);
    }
}

void send_request(node *head,node *usr)
{
    data new_data;
    node *p;
    bzero(&new_data,sizeof(new_data));
    printf("请输入想添加好友的ID\n");
    scanf("%d",&new_data.id);while(getchar()!='\n');
    p = find_friend_from_file(usr,new_data.id);
    if(p == usr)
    {
        printf("该好友已添加！\n");
        return;
    }
    p = find_node(head,new_data);
    if(p == NULL)
    {
        perror("该用户不存在！");
        return;
    }
    int cmd;
    printf("是否发送添加请求：1.是\t 2.否\n");
    scanf("%d",&cmd);while(getchar()!='\n');
    if(cmd == 2)
        return;
    if(cmd == 1)
    {
        msg_buf mydata;
        bzero(&mydata,sizeof(mydata));
        mydata.mtype = p->data.id;
printf("type  = %ld\n",mydata.mtype);
        mydata.mtext.id = usr->data.id;
        strncpy(mydata.mtext.type,"request",7);
        strncpy(mydata.mtext.name,usr->data.name,(int)strlen(usr->data.name));
        printf("请输入备注信息\n");
        fgets(mydata.mtext.mtext,sizeof(mydata.mtext.mtext),stdin);strtok(mydata.mtext.mtext,"\n");
        msgsnd(msg_id,&mydata,sizeof(mydata.mtext),0);
        printf("发送成功！\n");
    }
}   