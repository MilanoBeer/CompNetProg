#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUF_SIZE 100
#define SEQ_START 1000

/* Error */
#define ERR_FILE_NOT_FOUND -1

/* BOOLEAN */
#define FALSE 0
#define TRUE 1

typedef int bool_t;


typedef struct {
    int seq;
    int ack;
    int buf_len;
    char buf[BUF_SIZE];
} Packet;

void error_handling(char* message);

int main(int argc, char* argv[])
{
    Packet pck;
    struct sockaddr_in serv_addr;
    int sock, write_len, data_read_len, packet_read_len, file_size = 0;
    bool_t file_exist_flag = FALSE;
    char file_name[BUF_SIZE], buf[BUF_SIZE];
    int fd; 
    // check arg
    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    /* get client socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket() error");

    /* Init socket address */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

    /* connection */
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");

    printf("Input file name: ");
    scanf("%s", file_name);
    getchar();

    printf("[Client] request %s\n\n", file_name);
    // wrtie file_name to server --- (1)
    if ((write_len = write(sock, file_name, strlen(file_name))) == -1) 
        error_handling("file name write error!");
    
    // await get exist_flag from server --- (2)
    if ((data_read_len = read(sock, (void *)&file_exist_flag, sizeof(file_exist_flag)) == -1))
        error_handling("file exist flag read error!");

    /*    
     * if !exist_flag then
     *   not found error --- (*1)
    */
    if (!file_exist_flag) {
        puts("File Not Found");
        return ERR_FILE_NOT_FOUND;
    }

    // read initial packet information from server
    if (read(sock, (void *)&pck, sizeof(pck)) != sizeof(pck))
        error_handling("pck read error!");
    fd = creat(file_name, 0644);
    
    while ((data_read_len = read(sock, (void *)buf, BUF_SIZE)) != -1) {
        file_size += data_read_len;
        printf("[Client] Rx SEQ: %d, len: %3d bytes\n", pck.seq, data_read_len);
        pck.ack = pck.seq + data_read_len + 1;
	write(fd, (void *)buf, data_read_len);
        // loop breaking
        if (data_read_len != BUF_SIZE) break;

        if (write(sock, (void *)&pck, sizeof(pck)) != sizeof(pck))
            error_handling("pck write error!");
        printf("[Client] Tx ACK: %d\n\n", pck.ack);
        
        if (read(sock, (void *)&pck, sizeof(pck)) != sizeof(pck))
            error_handling("pck read error!");
    }
    printf("%s received (%d Bytes)\n", file_name, file_size);

    return 0;

}


void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

