#include"myhead.h"

void *recv(void *arg)
{
    pthread_detach(pthread_self());
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    node *pos = NULL;
    data new_data;
    node *usr = (node*)arg;
    msg_buf get_data,send_data;
    get_data.mtype = usr->data.id;
    msg_id = msgget(ftok(".",1),IPC_CREAT|0666);
    if(msg_id == -1)
    {
        perror("msgget failed!");
        return NULL;
    } 
    printf("recv\n"); 
    printf("msg_id = %d\n",msg_id);
    while(1)
    {
        bzero(&get_data.mtext,sizeof(get_data.mtext));
        msgrcv(msg_id,&get_data,sizeof(get_data.mtext),get_data.mtype,0);
        printf("%s\n",get_data.mtext.type);
        if(!strcmp(get_data.mtext.type,"request"))
        {
    printf("request\n"); 
            printf("ID%d 用户名%s的用户给您发送好友请求，备注信息:%s\n",get_data.mtext.id,get_data.mtext.name,get_data.mtext.mtext);
            printf("是否通过：666.是\t 888.否\n");
            printf("temp:%d\n",temp);
            while(!temp);
            {
                sleep(1);
                printf("temp:%d\n",temp);
            }
            if(temp == 888)
            {
                temp = 0;
                printf("\nNO\n");
                bzero(&send_data,sizeof(send_data));
                strncpy(send_data.mtext.type,"no",2);
            }
            if(temp == 666)
            {
                temp = 0;
                printf("\nYES\n");
                bzero(&send_data,sizeof(send_data));
                strncpy(send_data.mtext.type,"yes",3);
                fresh_friend_list(usr,get_data.mtext.id,get_data.mtext.name);
            }
            send_data.mtype = get_data.mtext.id;
            send_data.mtext.id = usr->data.id;
            strncpy(send_data.mtext.name,usr->data.name,(int)strlen(usr->data.name));
            msgsnd(msg_id,&send_data,sizeof(send_data.mtext),0);
        }
        else if(!strcmp(get_data.mtext.type,"friend"))
        {
    printf("friend\n"); 
            printf("好友%d : %s : %s\n",get_data.mtext.id,get_data.mtext.name,get_data.mtext.mtext);
        }
        else if(!strcmp(get_data.mtext.type,"yes"))
        {
            printf("好友请求通过!\n");
            fresh_friend_list(usr,get_data.mtext.id,get_data.mtext.name);
        }
        else if(!strcmp(get_data.mtext.type,"no"))  
        {
            printf("对方拒绝添加好友!\n");
        }
        else if(!strcmp(get_data.mtext.type,"recv"))
        {
            printf("ID%d name %s 接收了文件%s\n",get_data.mtext.id,get_data.mtext.name,get_data.mtext.mtext);
        }
        else if(!strcmp(get_data.mtext.type,"refuse"))
        {
            printf("ID%d name %s 拒绝接收文件%s\n",get_data.mtext.id,get_data.mtext.name,get_data.mtext.mtext);
        }
        else if(!strcmp(get_data.mtext.type,"world"))
        {
            printf("群聊 %d : %s : %s\n",get_data.mtext.id,get_data.mtext.name,get_data.mtext.mtext);
        }
        else if(!strcmp(get_data.mtext.type,"online"))
        {
            printf("online\n");
            if(get_data.mtext.id != usr->data.id)
                printf("ID:%d 用户:%s 上线!\n",get_data.mtext.id,get_data.mtext.name);
            bzero(&new_data,sizeof(new_data));
            new_data.id = get_data.mtext.id;
            strncpy(new_data.name,get_data.mtext.name,strlen(get_data.mtext.name));
            pos = find_node(online_usr,new_data);//在线用户节点中查找上线用户，若不存在就添加
            if(pos == NULL)
                kl_add_tail(new_data,online_usr);
        }
        else if(!strcmp(get_data.mtext.type,"quit"))
        {
            printf("quit\n");
            if(get_data.mtext.id != usr->data.id)
                printf("ID:%d 用户:%s 下线!\n",get_data.mtext.id,get_data.mtext.name);
            bzero(&new_data,sizeof(new_data));
            new_data.id = get_data.mtext.id;
            pos = find_node(online_usr,new_data);
            if(pos != NULL)
            {
                printf("%d %s\n",pos->data.id,pos->data.name);
                kl_del(new_data,online_usr);
            }
        }
        else if(!strcmp(get_data.mtext.type,"file"))
        {
            printf("file\n");
            printf("好友ID%d name %s 给你发送了名为 %s 的文件\n",get_data.mtext.id,get_data.mtext.name,get_data.mtext.mtext);
            printf("是否接收：111.是 222.否\n");
            while(!temp);
            {
                printf("temp:%d\n",temp);
            }
            if(temp == 111)
            {
                printf("请输入文件保存路径\n");
                filename get_name;
                bzero(&get_name,sizeof(get_name));
                strncpy(get_name.src,get_data.mtext.mtext,(int)strlen(get_data.mtext.mtext));
                pthread_mutex_lock(&lock);
                fgets(get_name.dest,sizeof(get_name.dest),stdin);strtok(get_name.dest,"\n");
                pthread_mutex_unlock(&lock);
                cp(get_name.src,get_name.dest);
                bzero(&send_data,sizeof(send_data));
                strncpy(send_data.mtext.type,"recv",4);
                printf("send_data.mtext.type: %s\n",send_data.mtext.type);
                temp = 0;
            }
            if(temp == 222)
            {
                temp = 0;
                printf("\nrefuse\n");
                bzero(&send_data,sizeof(send_data));
                strncpy(send_data.mtext.type,"refuse",6);
                printf("send_data.mtext.type: %s\n",send_data.mtext.type);
            }
            send_data.mtype = get_data.mtext.id;
            printf("send to id : %d\n",get_data.mtext.id);
            send_data.mtext.id = usr->data.id;
            strncpy(send_data.mtext.name,usr->data.name,(int)strlen(usr->data.name));
            strncpy(send_data.mtext.mtext,get_data.mtext.mtext,(int)strlen(get_data.mtext.mtext));
            msgsnd(msg_id,&send_data,sizeof(send_data.mtext),0);
        }
    }
}
 
node *login(node *head)
{
    printf("请输入用户id\n");
    data get_info;
    scanf("%d",&get_info.id);while(getchar()!='\n');
    node *p = find_node(head,get_info);
    if(p == NULL)
    {
        printf("该id不存在！\n");
        return NULL;
    }
   /* node *q = find_node(online_usr,get_info);
    if(q != NULL)
    {
        printf("该账号已在登录状态！\n");
        return NULL;
    }*/
    int n = 3;
    while(n--)
    {
        printf("请输入密码\n");
        fgets(get_info.pwd,20,stdin);strtok(get_info.pwd,"\n");
        if(!strcmp(get_info.pwd,p->data.pwd))
        {
            printf("登录成功！\n");
            return p;
        }
        else
        {
            printf("密码错误，请重新输入！\n");
        }
    }
    printf("密码输错三次，建议重置密码！\n");
    return NULL;
}

void usr_add(node *head)
{
    data new_data;
    bzero(&new_data,sizeof(new_data));
    printf("请输入用户id\n");
    scanf("%d",&new_data.id);while(getchar()!='\n');
    node *p = find_node(head,new_data);
    if(p != NULL)
    {
        printf("该id已存在！\n");
        return;
    }

    printf("请输入用户名\n");
    fgets(new_data.name,15,stdin);strtok(new_data.name,"\n");
    printf("请设置密码\n");
    fgets(new_data.pwd,10,stdin);strtok(new_data.pwd,"\n");
    kl_add_tail(new_data,head);
    add_info_to_file(new_data);
    return;
}

void update_info(node *head,node *usr)
{
    int cmd;
    data p;
    while(1)
    {
        printf("0.退出\n 1.修改密码\n 2.修改用户名\n");
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 0)    break;
        switch(cmd)
        {
            case 1: 
                    printf("请输入新的密码\n");
                    fgets(usr->data.pwd,20,stdin);strtok(usr->data.pwd,"\n");
                    break;
            case 2: 
                    printf("请输入新的用户名\n");
                    fgets(usr->data.name,20,stdin);strtok(usr->data.name,"\n");
                    break;
            default: break;
        }
    }
    empty_and_rewrite_info_to_file(head);
    return;
}

void send_file_private(node *usr)
{
    msg_buf mydata;
    bzero(&mydata,sizeof(mydata));
    data new_data;
    node *p;
    bzero(&new_data,sizeof(new_data));
    strncpy(mydata.mtext.type,"file",4);
    strncpy(mydata.mtext.name,usr->data.name,(int)strlen(usr->data.name));
    printf("请输入要发送的文件\n");
    fgets(mydata.mtext.mtext,sizeof(mydata.mtext.mtext),stdin);strtok(mydata.mtext.mtext,"\n");
    printf("请输入要发送文件的好友ID\n");
    scanf("%d",&new_data.id);while(getchar()!='\n');
    p = find_node(online_usr,new_data);
    if(p == NULL)
    {
        perror("该用户不在线！");
        return;
    }
    mydata.mtype = new_data.id;
    mydata.mtext.id = usr->data.id;
    msgsnd(msg_id,&mydata,sizeof(mydata.mtext),0);
}

void send_file_all(node *usr)
{
    msg_buf mydata;
    node *pos,*n;
    bzero(&mydata,sizeof(mydata));
    mydata.mtext.id = usr->data.id;
    strncpy(mydata.mtext.type,"file",4);
    strncpy(mydata.mtext.name,usr->data.name,(int)strlen(usr->data.name));
    printf("请输入要发送的文件\n");
    fgets(mydata.mtext.mtext,sizeof(mydata.mtext.mtext),stdin);strtok(mydata.mtext.mtext,"\n");
    list_for_each_entry_safe(pos,n,&online_usr->list,list)
    {
        mydata.mtype = pos->data.id;
        if(pos->data.id != usr->data.id)
            msgsnd(msg_id,&mydata,sizeof(mydata.mtext),0);
    }
}