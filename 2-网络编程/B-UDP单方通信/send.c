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

    struct sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(atoi(argv[2]));
    destaddr.sin_addr.s_addr = inet_addr(argv[1]);
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