#include"myhead.h"
//传入自己的IP和端口号
int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        perror("input error!");
        return -1;
    }
    int sockfd = sockfd_init(argv[2]);
    node *head = init();
//头节点存放自己的IP和端口号
    head->user_data.port = atoi(argv[2]);
    strcpy(head->user_data.ip,argv[1]);
    struct sockaddr_in destaddr;
    socklen_t len = sizeof(destaddr);
    char  buf[50];
    char temp[10];
    data new_data;
    node *pos;
    int n;
    while(1)
    {
        bzero(buf,sizeof(buf));
        recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,&len);
        bzero(&new_data,sizeof(new_data));
        strcpy(new_data.ip,inet_ntoa(destaddr.sin_addr));
        new_data.port = ntohs(destaddr.sin_port);
        pos = find_node(head,new_data);//通过ip和端口号查找用户
        if(pos == NULL)//若该用户不存在，就注册
        {
            strncpy(new_data.name,buf,30);//用户第一次注册，发的消息为用户名
            add_tail(head,new_data);
            show(head);
            continue;
        }
        if(!strncmp(buf,"list",4))//打印在线用户信息
        {
            show_list_to_usr(sockfd,head,pos);
            continue;
        }
        else if(!strcmp(buf,"quit"))//某个客户端退出程序
        {
            bzero(&new_data,sizeof(new_data));
            strcpy(new_data.ip,inet_ntoa(destaddr.sin_addr));
            new_data.port = ntohs(destaddr.sin_port);
            del(head,new_data);//从链表删除该客户端信息
            continue;
        }
        // bcast:hello		发送广播消息
	    // group:hello		发送组播消息
        n = detect_colon(buf);
        if(n != -1)
        {
           strncpy(temp,buf,n);
           if(!strncmp(temp,"bcast",5))//广播
           {
               printf("temp  bcast:%s\n",temp);
               bcast_and_group_send(sockfd,pos,buf,"192.168.1.255",pos->user_data.port);
               continue;
           }
           else if(!strncmp(temp,"group",5))//组播
           {
               printf("temp group:%s\n",temp);
               bcast_and_group_send(sockfd,pos,buf,"224.0.0.10",pos->user_data.port);
               continue;
           }
           else
           {
               send_to_someone(sockfd,head,pos,buf);
               continue;
           }
        }
        printf("from [%s]<%d> : %s\n",pos->user_data.ip,pos->user_data.port,buf);
    }
    return 0;
}

int detect_colon(char *str)
{
	int i;
	for(i=0;i<strlen(str);i++)
		if(str[i] == ':')
			return i;
	return -1;
}

int sockfd_init(char *port)
{
    int udpfd = socket(AF_INET,SOCK_DGRAM,0);
    if(udpfd == -1)
    {
        perror("socket failed!");
        return -1;
    }
    int n = 1;
    setsockopt(udpfd,SOL_SOCKET,SO_BROADCAST,&n,sizeof(n));
    struct ip_mreq a;
    a.imr_multiaddr.s_addr = inet_addr("224.0.0.10");
    a.imr_interface.s_addr = inet_addr("192.168.1.164");
    setsockopt(udpfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&a,sizeof(a));
/*
    要实现对等通信，则双方都要绑定自己的IP和端口号，
    若不绑定，则端口号每次运行会由系统自行分配，导致端口号的值不确定
    但开始时一方给另一方发送消息的前提是，明确对方的 IP和端口号
*/
    struct sockaddr_in myaddr;
    //INADDR_ANY = 0.0.0.0 表示任意网卡ip，可接收任意IP发来的消息
    myaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(port));
    bind(udpfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
    return udpfd;
}

void show_list_to_usr(int sockfd,node *head,node *usr)
{
    node *p;
    char buf[80];
    for(p = head->next;p != NULL;p = p->next)
    {
        sprintf(buf,"IP:%s port:%d name:%s",p->user_data.ip,p->user_data.port,p->user_data.name);
        bcast_and_group_send(sockfd,usr,buf,usr->user_data.ip,usr->user_data.port);
    }
    return;
}

void send_to_someone(int sockfd,node *head,node *usr,char *buf)
{
    char name[30];
    char msg[30];
    printf("buf:%s\n",buf);
    bzero(name,sizeof(name));
    bzero(msg,sizeof(msg));
    int n = detect_colon(buf);
    //将发消息的用户名与消息正文进行拼接
    strcpy(msg,&buf[n+1]);
    strncpy(name,buf,n);
    sprintf(buf,"%s:%s",usr->user_data.name,msg);
    printf("buf:%s\n",buf);
    node *pos;
    for(pos = head->next;pos != NULL;pos = pos->next)
    {
        if(!strcmp(pos->user_data.name,name))
        {//找到指定用户，给她发送消息
            printf("pos->ip:%s\n",pos->user_data.ip);
            bcast_and_group_send(sockfd,usr,buf,pos->user_data.ip,pos->user_data.port);
            return;
        }
    }
    // printf("该用户不存在！\n");
    return;
}

/*
sockfd:通信用套接字
usr：要发送消息的用户节点
buf：要发送的消息
ip：接收者ip
port：接收者端口号
*/
void bcast_and_group_send(int sockfd,node *usr,char *buf,char *ip,int port)
{
    struct sockaddr_in destaddr;
    bzero(&destaddr,sizeof(destaddr));
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(port);
    destaddr.sin_addr.s_addr = inet_addr(ip);
    char temp[70];
    if(strcmp(usr->user_data.ip,ip) == 0 && usr->user_data.port == port)
        strcpy(temp,buf);
    else
        sprintf(temp,"[%s]:%s",usr->user_data.ip,buf);
    int ret;
    ret = sendto(sockfd,temp,sizeof(temp),0,(struct sockaddr*)&destaddr,sizeof(destaddr));
}