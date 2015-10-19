#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<errno.h>
extern record(int,char *);

int main(int argc,char *argv[])
{
	system("clear");
	printf("\n\t\t\t\t\tServer\t\t\t\t\n");

	int connfd,sockfd,n,n1,clen,pid;
	FILE *fp;

	struct sockaddr_in servaddr,cliaddr;

	char *serverString,filename[50];

	bzero(&servaddr,sizeof(servaddr));
	bzero(&cliaddr,sizeof(cliaddr));
	
	sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//socket 

	if(sockfd < 0)
	{
		printf("\n\tSocket Error\n");
		perror("\n\tSocket");
		exit(1);
	}

	serverString = (char *)malloc(1024);	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5579);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if((bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))) < 0)//bind func
	{
		printf("\n\tBind Error\n");
		perror("\n\tBind");
		exit(1);
	}
	
	if((listen(sockfd,3)) < 0)//listen
	{
		printf("\n\tListen Error\n");
		perror("\n\tListen");
		exit(1);
	}
	
	while(1)
	{
		clen = sizeof(cliaddr);
		connfd = accept(sockfd,(struct sockaddr *)&cliaddr,&clen);
		if(connfd < 0)
		{
			printf("\n\tAccept Error\n");
			perror("\n\tAccept");
			exit(1);
		}
		
		if((pid = fork()) == 0)
		{
			//close(sockfd);
			bzero(serverString,1024);
			
			label1 : if((n1 = read(connfd,serverString,1024)) > 0)
			{
				sleep(5);
				record(1024,serverString);
				//bzero(serverString,1024);
			}
			if((n1 < 0) && (errno == EINTR))
			{
				goto label1;
			}
			if(n1 < 0)
			{
				printf("\n\tRead Error\n");
				perror("\n\tRead");
				//exit(1);
			}
		}
		close(connfd);
	}
}
