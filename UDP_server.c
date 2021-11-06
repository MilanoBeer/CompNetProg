 /* 
 2018117460 글솝 신혜연
 socket option inquiry program / udp 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define BUF_SIZE 256

typedef struct {
    int optnum; // option name 
    int optval; 
    int result; 
}SO_PACKET; 

void error_handling(char *message);
int main(int argc, char *argv[])
{
    int udp_sock, tcp_sock, clnt_sock; 
    int sock_type; // 값을 받아올 변수 
    socklen_t optlen; // 변수에 대한 크기를 넣음   
    int state; 
    //SO_PACKET pck = {{0, }, 0, 0, 0};
    SO_PACKET pck = {0, };
    int send_len, rcv_len; 
    socklen_t clnt_addr_sz;
    struct sockaddr_in serv_addr, clnt_addr;

    optlen = sizeof(sock_type);

    /* UDP, TCP Socket 할당 */
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(udp_sock == -1)
        error_handling("udp_sock() error");

    tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(tcp_sock == -1)
        error_handling("tcp_sock() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));       

    if (bind(udp_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

	if (bind(tcp_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

    /* ready to get connection signal */
	if (listen(tcp_sock, 5) == -1)
		error_handling("listen() error");

    /* receive from Client, */
    clnt_addr_sz = sizeof(clnt_addr);
    printf("Socket Option Server Start\n");
    
    while(1)
    {
        if ((rcv_len = recvfrom(udp_sock, (void *)&pck, BUF_SIZE, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_sz)) == -1)
            error_handling("recv() error");    
        
        if(pck.optnum == 1){
            printf(">>> Received Socket option: SO_SNDBUF\n");
            state = getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&sock_type, &optlen);           
            
            printf("Send option : SO_SNDBUF: %d, result : %d\n\n", sock_type, state);
            
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");
        }

        else if(pck.optnum == 2){
            printf(">>> Received Socket option: SO_RCVBUF\n");
            state = getsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&sock_type, &optlen);           
            
            printf("Send option : SO_RCVBUF: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");            
        }

        else if(pck.optnum == 3){
            printf(">>> Received Socket option: SO_REUSEADDR\n");
            state = getsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_type, &optlen);           
            
            printf("Send option : SO_REUSEADDR: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");                        
        }       
        
        else if(pck.optnum == 4){
            printf(">>> Received Socket option: SO_KEEPALIVE\n");
            state = getsockopt(udp_sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&sock_type, &optlen);           
            
            printf("Send option : SO_KEEPALIVE: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");                    
        }

        else if(pck.optnum == 5){
            printf(">>> Received Socket option: SO_BROADCASE\n");
            state = getsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, (void*)&sock_type, &optlen);           
            
            printf("Send option : SO_BROADCAST: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");                
        }

        else if(pck.optnum == 6){
            printf(">>> Received Socket option: IP_TOS\n");
            state = getsockopt(udp_sock, IPPROTO_IP, IP_TOS, (void*)&sock_type, &optlen);           
            
            printf("Send option : IP_TOS: %d, result : %d\n\n", sock_type, state);

            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");   
        }

        else if(pck.optnum == 7){
            printf(">>> Received Socket option: IP_TTL\n");
            state = getsockopt(udp_sock, IPPROTO_IP, IP_TTL, (void*)&sock_type, &optlen);           
            
            printf("Send option : IP_TTL: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");   
        }

        else if(pck.optnum == 8){      
            printf(">>> Received Socket option: TCP_NODELAY\n");

            int opt_val = 0; 
            setsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val));

            socklen_t opt_len; 
            opt_len = sizeof(opt_val);           
            state = getsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&sock_type, &optlen);           
            
            printf("Send option : TCP_NODELAY: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 

            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");   
        }

        else if(pck.optnum == 9){
            printf(">>> Received Socket option: TCP_MAXSEG\n");

            int opt_val = 1; 
            setsockopt(tcp_sock, IPPROTO_TCP, TCP_MAXSEG, (void*)&opt_val, sizeof(opt_val));

            socklen_t opt_len; 
            opt_len = sizeof(opt_val);           
            state = getsockopt(tcp_sock, IPPROTO_TCP, TCP_MAXSEG, (void*)&sock_type, &optlen);     
            //state = getsockopt(udp_sock, IPPROTO_TCP, TCP_MAXSEG, (void*)&sock_type, &optlen);           
            
            printf("Send option : TCP_MAXSEG: %d, result : %d\n\n", sock_type, state);
            pck.optval = sock_type;
            pck.result = state; 
            if ((send_len = sendto(udp_sock, (void *)&pck, sizeof(pck), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
                error_handling("sendto() error");   
        }
    }
    return 0; 
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
