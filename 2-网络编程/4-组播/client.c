#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>

int main(int argc,char **argv)
{   
    int udpfd = socket(AF_INET,SOCK_DGRAM,0);
    if(udpfd == -1)
    {
        perror("socket failed!");
        return -1;
    }

    struct ip_mreq a;
    a.imr_multiaddr.s_addr = inet_addr("224.0.0.100");
    a.imr_interface.s_addr = inet_addr("192.168.1.164");
    setsockopt(udpfd,IPPROTO_TCP,IP_ADD_MEMBERSHIP,&a,sizeof(a));
    struct sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(atoi(argv[1]));
    destaddr.sin_addr.s_addr = inet_addr("224.0.0.100");
    socklen_t len = sizeof(destaddr);
    char buf[50];
    while(1)
    {
        bzero(buf,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);strtok(buf,"\n");
        sendto(udpfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,len);
        if(!strcmp(buf,"quit"))
            break;
    }
    close(udpfd);
    return 0;
}

