#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

void error_handling(char* message);

int main(int argc, char* argv[])
{
	int client_sock;
	struct sockaddr_in serv_addr;
	char address[30];
	char rcv_message[64];
	char quit_m[] = "quit!\n";
	int str_len;

	if(argc != 3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	// 1step : make socket
	client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	//ask connect to server 
	if(connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	while(1)
	{
		printf("Input dotted-decimal address: ");
		scanf("%s", address);
		write(client_sock, address, sizeof(address));
	
		read(client_sock, rcv_message, sizeof(rcv_message)-1);
		if(!strcmp(rcv_message,quit_m))
		{
			printf("%s \n", rcv_message); 
			break;
		}
		else
		{
			printf("%s \n",rcv_message); 
		} 
	}		

	close(client_sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
} 

