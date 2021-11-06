#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

#define BUF_SIZE 100
#define SEQ_START 1000

/* Error */
#define ERR_FILE_NOT_FOUND -1

/* BOOLEAN */
#define FALSE 0
#define TRUE 1

typedef int bool_t;

/* PACKET */
typedef struct {
        int seq;
        int ack;
        int buf_len;
        char buf[BUF_SIZE];
        }Packet;

void error_handling(char *message);

int main(int argc, char *argv[])
{
    Packet pck = {
        SEQ_START, 
        0, 
        0, 
        {0, }};
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    int serv_socket, clnt_sock, write_len, data_read_len, packet_read_len, fd, file_size = 0;
    bool_t file_exist_flag = FALSE;
    char file_name[BUF_SIZE] = {0, }, buf[BUF_SIZE] = {0, };

    // check arg
    if (argc != 2) {
        printf("Usage :  %s <port>\n", argv[0]);
        exit(1);
    }

    /* get server socket */
	if ((serv_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket() error");

	/* socket address setting to use */
	memset(&serv_addr, 0, sizeof(serv_addr)); // make dump to zero
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	/* binding */
	if (bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	/* ready to get connection signal */
	if (listen(serv_socket, 5) == -1)
		error_handling("listen() error");

    puts("----------------------------------");
    puts("    File Transmission Server");
    puts("----------------------------------");

    if ((clnt_sock = accept(serv_socket, (struct sockaddr*)&clnt_addr, &clnt_addr_size)) == -1)		
        error_handling("accept() error");

    // await read file_name from client --- (1)
    if ((data_read_len = read(clnt_sock, file_name, BUF_SIZE)) == -1)
        error_handling("file name read error!");
    
    // check exist file
    if ((fd = open(file_name, O_RDONLY)) != -1)
        file_exist_flag = TRUE;
    
    // write exist_flag to client --- (2)
    if ((write_len = write(clnt_sock, (void *)&file_exist_flag, sizeof(file_exist_flag))) == -1) 
        error_handling("file exist flag write error!");

    /*
        if !file exist {
            not found error --- (*1)
        }
    */
    if (fd == -1) {
        printf("%s File Not Found\n", file_name);
        return ERR_FILE_NOT_FOUND;
    }

    printf("[Server] sending %s\n\n", file_name);

    // write init packet_info to client
    if (write(clnt_sock, (void *)&pck, sizeof(pck)) != sizeof(pck))
        error_handling("pck write error!");

    while ((data_read_len = read(fd, (void *)buf, sizeof(buf))) != -1) {
        if ((write_len = write(clnt_sock, (void *)buf, data_read_len)) == -1) 
            error_handling("write error!");
        file_size += write_len;
        printf("[Server] Tx: SEQ: %d, %3d byte data\n", pck.seq, write_len);

        if (write_len != BUF_SIZE) break;

        // await
        if (read(clnt_sock, (void *)&pck, sizeof(pck)) != sizeof(pck)) 
            error_handling("packet read error");
        printf("[Server] Rx: ACK: %d\n\n", pck.ack);
        
        // packet info transformation
        pck.seq = pck.ack;

        if (write(clnt_sock, (void *)&pck, sizeof(pck)) == -1)
            error_handling("pck write error!");
    }

    printf("%s sent (%d Bytes)\n\n", file_name, file_size);

    close(fd);
    close(serv_socket);  
    close(clnt_socket);  
    return 0;
}

void error_handling(char* message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}
