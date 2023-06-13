#include"myhead.h"

pthread_t recv_p_tid;

node *run(node *head)
{
    int cmd;
    node *pos;
    node *usr;
    int n;
    while(1)
    {
        printf("==========================================\n");
        printf("==                1.登录                ==\n");
        printf("==                2.注册                ==\n");
        printf("==                3.退出                ==\n");
        printf("==========================================\n");
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 3)
            break;
        if(cmd == 1)
        {
            online_usr = kl_init();
            if(online_usr == NULL)
                    return NULL;
            usr = login(head);
            if(usr != NULL)
            {
                read_file_to_ulink(online_usr);
                pos = find_usr_from_ufile(online_usr);
                if(pos == NULL)
                    fresh_now_usr(usr);//往文件追加在线用户信息
                //接收消息
                n = pthread_create(&recv_p_tid,NULL,recv,(void*)&(usr->data.id));
                if(n != 0)
                {
                    printf("error\n");
                }
                on_or_un_line_send(head,usr,"online");//向服务器发送上线消息
                panel(head,usr);
                empty_and_rewrite_info_to_ufile(online_usr);
                destory_klist(online_usr);  
                pthread_cancel(recv_p_tid);
            }
        }
        if(cmd == 2)
            usr_add(head);
    }
}

void panel(node *head,node *usr)
{
    int cmd;
    int flag1 = 0;
    while(1)
    {
        printf("=================首页=====================\n");
        printf("==              1.私聊                  ==\n");
        printf("==              2.群聊                  ==\n");
        printf("==              3.添加好友              ==\n");
        printf("==              4.修改信息              ==\n");
        printf("==              5.发送文件              ==\n");
        printf("==              6.退出登录              ==\n");
        printf("==========================================\n");
        if(flag1 == 0)
        {
            flag1 = 1;
            pthread_mutex_lock(&lock);
        }
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 6)
        {
            pthread_mutex_unlock(&lock);
            on_or_un_line_send(head,usr,"quit");
            usr_show(online_usr);
            // sleep(2);
            usr_show(online_usr);
            if(list_empty(&online_usr->list))
            {
                empty_now_usr_file();
                int ret = msgctl(msg_id,IPC_RMID,NULL);
                if(ret != 0)
                    perror("panel msgrm failed!");
            }
            break;
        }
        switch(cmd)
        {
            case 1: pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    private_chat(head,usr);
                    break;
            case 2: pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    group_chat(head,usr);
                    break;
            case 3: pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    friend_request(head,usr);
                    break;
            case 4: pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    update_info(head,usr);
                    break;
            case 5: pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    send_file(usr);
                    break;
            case 666 : temp = 666;
                    pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    break;
            case 888 : temp = 888;
                    pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    break;
            case 111: temp = 111;
                    pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    while(temp != 0);
                    break;
            case 222: temp = 222;
                    pthread_mutex_unlock(&lock);
                    flag1 = 0;
                    break;
            default:
                    break;
        }
    }
}

void send_file(node *usr)
{
    int cmd;
    int flag1 = 0;
    while(1)
    {
        printf("==========================================\n");
        printf("==              1.私发                  ==\n");
        printf("==              2.群发                  ==\n");
        printf("==              3.退出                  ==\n");
        printf("==========================================\n");
        if(flag1 == 0)
        {
            pthread_mutex_lock(&lock);
            flag1 = 1;
        }
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 3)
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        if(cmd == 1)
        {
            pthread_mutex_unlock(&lock);
            flag1 = 0;
            send_file_private(usr);
        }
        else if(cmd == 2)
        {
            pthread_mutex_unlock(&lock);
            flag1 = 0;
            send_file_all(usr);
        }
        if(cmd == 666)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 888)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 111)
        {
            temp = 111;
            pthread_mutex_unlock(&lock);
            while(temp != 0);
            flag1 = 0;
        }
        if(cmd == 222)
        {
            temp = 222;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
    }
}

void friend_request(node *head,node *usr)
{
    int cmd;
    int flag1 = 0;
    while(1)
    {
        printf("==========================================\n");
        printf("==          1.查看当前在线用户           ==\n");
        printf("==          2.直接添加好友               ==\n");
        printf("==          3.退出                      ==\n");
        printf("==========================================\n");
        if(flag1 == 0)
        {
            pthread_mutex_lock(&lock);
            flag1 = 1;
        }
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 3)
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        if(cmd == 1)
        {
            pthread_mutex_unlock(&lock);
            flag1 = 0;
            print_usr(online_usr);
        }
        if(cmd == 666)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 888)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 111)
        {
            temp = 111;
            pthread_mutex_unlock(&lock);
            while(temp != 0);
            flag1 = 0;
        }
        if(cmd == 222)
        {
            temp = 222;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 2)
        {
            pthread_mutex_unlock(&lock);
            flag1 = 0;
            send_request(head,usr);
        }
    }
}

void group_chat(node *head,node *usr)
{
    msg_buf mydata;
    node *pos,*n;
    int flag1 = 0;
    while(1)
    {
        bzero(&mydata,sizeof(mydata));
        mydata.mtext.id = usr->data.id;
        strncpy(mydata.mtext.name,usr->data.name,(int)strlen(usr->data.name));
        strncpy(mydata.mtext.type,"world",5);
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
        list_for_each_entry_safe(pos,n,&head->list,list)//给所有用户发送
        {
            mydata.mtype = pos->data.id;
printf("type  = %ld\n",mydata.mtype);
            if(find_usr_from_ufile(pos) != NULL)
                msgsnd(msg_id,&mydata,sizeof(mydata.mtext),0);
        }
    }
}

void private_chat(node *head,node *usr)
{
    int cmd;
    int flag1 = 0;
    while(1)
    {
        printf("==========================================\n");
        printf("==          1.查看我的好友               ==\n");
        printf("==          2.选择好友私聊               ==\n");
        printf("==          3.退出                      ==\n");
        printf("==========================================\n");
        if(flag1 == 0)
        {
            pthread_mutex_lock(&lock);
            flag1 = 1;
        }
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 3)
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        if(cmd == 1)
        {
            pthread_mutex_unlock(&lock);
            flag1 = 0;
            print_frineds(usr);
        }
        if(cmd == 666)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 888)
        {
            temp = 666;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 111)
        {
            temp = 111;
            pthread_mutex_unlock(&lock);
            while(temp != 0);
            flag1 = 0;
        }
        if(cmd == 222)
        {
            temp = 222;
            pthread_mutex_unlock(&lock);
            flag1 = 0;
        }
        if(cmd == 2)
        {
            pthread_mutex_unlock(&lock);
            flag1 = 0;
            send_msg(head,usr);
        }
    }
}
