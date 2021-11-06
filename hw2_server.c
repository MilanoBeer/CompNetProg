#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUF_SIZE 128 

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	int str_len;
	 
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	struct sockaddr_in addr_inet; 

	socklen_t clnt_addr_size;
	unsigned long conv_addr;
	
	char buff_rcv[BUF_SIZE]; 
	char buff_snd[BUF_SIZE];
	char* buff_quit = "quit";

	char fail_m[] = "Address conversion fail: Format error\n";
	char succ_m[] = "Address conversion success\n";
	char quit_m[] = "quit!\n";  
	char* str_ptr; 	
		
	if(argc != 2) {
		printf("Usage :  %s <port>\n", argv[0]);
		exit(1);
	}

	//1 step : make socket
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	// 2 setp : bind
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error");

	// 3 step: listen
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_addr_size = sizeof(clnt_addr);

	// 4 step: accept connect from client  
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

	if(clnt_sock == -1)
		error_handling("accept() error");

	printf("-------------------------\n");
	printf("Address Conversion Server\n");
	printf("------------------------\n");

	while(1)
	{
		read(clnt_sock, buff_rcv, BUF_SIZE);
		if(!strcmp(buff_rcv,buff_quit))
		{		
			printf("quit received and exit program!\n");
			write(clnt_sock, quit_m, sizeof(quit_m));
			break;
		}
 
		printf("Received Dotted-Decimal Address:  : %s \n", buff_rcv);
		conv_addr = inet_addr(buff_rcv);
		if(conv_addr == INADDR_NONE)
		{
			printf("%s \n", fail_m);
			
			write(clnt_sock, fail_m, sizeof(fail_m));
			continue; 					
		}

		if(!inet_aton(buff_rcv, &addr_inet.sin_addr))
			error_handling("Conversion error");
		else
			printf("inet_aton: %s -> %#x \n", buff_rcv, addr_inet.sin_addr.s_addr);
	
		str_ptr = inet_ntoa(addr_inet.sin_addr);
		strcpy(buff_rcv, str_ptr);
		printf("inet_ntoa: %#x -> %s \n", addr_inet.sin_addr.s_addr, str_ptr);
		printf("%s\n", succ_m);
		write(clnt_sock, succ_m, sizeof(succ_m));	
	}	

	close(clnt_sock);
	close(serv_sock);
	return 0;
}		

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


