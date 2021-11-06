/* 2018117460 글솝 신혜연 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define BUF_SIZE 256

typedef struct {
    int optnum; 
    int optval; 
    int result; 
}SO_PACKET; 

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int udp_sock, tcp_sock;  
    int send_len, rcv_len; 
    socklen_t addr_sz;
    struct sockaddr_in serv_addr, from_addr;
    int Num = -1 ;  
    SO_PACKET pck = {0, };  

    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(udp_sock == -1)
        error_handling("udp_sock() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    while(1)
    {   
        /* Print Option List */
        printf("-------------------------\n");
        printf("1: SO_SNDBUF\n2: SO_RCVBUF\n3: SO_REUSEADDR\n4: SO_KEEPALIVE\n5: SO_BROADCAST\n6: IP_TOS\n7: IP_TTL\n8: TCP_NODELAY\n9: TCP_MAXSEG\n10: QUIT\n");
        printf("-------------------------\n");
    
        /* Input Option Number*/
        while(1)
        {
            printf("Input option number: ");
            scanf("%d", &Num);
            if(Num < 0 || Num > 10){
                printf("Wrong number. type again!\n");
                continue; 
            }
            else
                break;
        }

        pck.optnum = Num; 
        if((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1)
            error_handling("Sendto() error");

        if( Num == 10 ){
            printf("Client quit.\n");
            exit(1);
            // 서버로 데이터 전송x, Client만 종료 
        }
        
        addr_sz = sizeof(from_addr);                         
        if((rcv_len = recvfrom(udp_sock, (void *)&pck, BUF_SIZE, 0, (struct sockaddr *)&from_addr, &addr_sz)) == -1)
            error_handling("Sendto() error");
        if(Num == 1){
            printf(">>> Server result: SO_SNDBUF: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 2) {
            printf(">>> Server result: SO_RCVBUF: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 3){
            printf(">>> Server result: SO_REUSEADDR: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 4){
            printf(">>> Server result: SO_KEEPALIVE: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 5){
            printf(">>> Server result: SO_BROADCAST: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 6){
            printf(">>> Server result: IP_TOS: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 7){
            printf(">>> Server result: IP_TTL: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 8){
           printf(">>> Server result: TCP_NODELY: value : %d, result : %d\n", pck.optval, pck.result);
        }

        else if(Num == 9){
           printf(">>> Server result: TCP_MAXSEG: value : %d, result : %d\n", pck.optval, pck.result);
        }
    }

    close(udp_sock);
    return 0; 
    
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
