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
    a.imr_interface.s_addr = inet_addr("0.0.0.0");
    setsockopt(udpfd,IPPROTO_TCP,IP_ADD_MEMBERSHIP,&a,sizeof(a));

    struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port   = htons(50000);
	myaddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("0.0.0.0")				
	
	int ret = bind(udpfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	if(ret == -1)
	{
		perror("bind failed");
		return -1;
	}

    struct sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    socklen_t len = sizeof(destaddr);
    char buf[50];
    while(1)
    {
        bzero(buf,sizeof(buf));
        recvfrom(udpfd,buf,sizeof(buf),0,(struct sockaddr*)&destaddr,&len);
        printf("[%s]:%s\n",inet_addr(&destaddr.sin_addr),buf);
        if(!strcmp(buf,"quit"))
            break;
    }
    close(udpfd);
    return 0;
}

