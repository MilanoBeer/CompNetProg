/* 
HW4_
글로벌소프트웨어융합전공 2018117460 신혜연
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BOARD_SIZE 3
#define BUF_SIZE 50
#define INIT_VALUE 0
#define S_VALUE 1
#define C_VALUE 2
typedef struct
{
    int board[BOARD_SIZE][BOARD_SIZE];
} GAMEBOARD;

void draw_board(GAMEBOARD *gboard);
void error_handling(char *message);
int available_space(GAMEBOARD *gboard);

int main(int argc, char *argv[])
{
    int sock;
    int send_len, rcv_len;
    socklen_t addr_sz;
    struct sockaddr_in serv_addr, from_addr;
    GAMEBOARD Board = {
        0,
    };
    int space_result;
    int row = 5, col = 5;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    /*Start Client, print Board empty state*/
    printf("Tic-Tac-Toe Client\n");
    draw_board(&Board);

    while (1)
    {
        /*input row, column idx
            if idx is out of Board size, 
        again input idx*/
        while (1)
        {
            printf("Input row, column: ");
            scanf("%d %d", &row, &col);

            if (row >= BOARD_SIZE || col >= BOARD_SIZE)
                continue;
            else if (Board.board[row][col] != 0)
                continue;
            else
                break;
        }
        /*input 2 in borad, draw board state*/
        Board.board[row][col] = 2;
        draw_board(&Board);

        /* send Board-info to Server*/
        if ((send_len = sendto(sock, (void *)&Board, sizeof(Board), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1)
            error_handling("sendto() error");

        /* if Board is full */
        if ((space_result = available_space(&Board)) == 0)
        {
            if ((send_len = sendto(sock, (void *)&Board, sizeof(Board), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1)
                error_handling("sendto() error");

            printf("No available space. Exit Client.Tic Tac Toe Client Close\n");
            break;
        }

        /* receive from Server Board-info*/
        addr_sz = sizeof(from_addr);
        if ((rcv_len = recvfrom(sock, (void *)&Board, BUF_SIZE, 0, (struct sockaddr *)&from_addr, &addr_sz)) == -1)
            error_handling("recvfrom() error");
        draw_board(&Board);
    }
    close(sock);
    return 0;
}

int available_space(GAMEBOARD *gboard)
{
    int i, j;
    int result = 0;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (gboard->board[i][j] == 0)
            {
                result = 1;
                break;
            }
        }
    }
    return result;
}

void draw_board(GAMEBOARD *gboard)
{
    char value = ' ';
    int i, j;

    printf("+-----------+\n");
    for (i = 0; i < BOARD_SIZE; i++)
    {
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (gboard->board[i][j] == INIT_VALUE) // 초기값 0
                value = ' ';
            else if (gboard->board[i][j] == S_VALUE) // Server 1
                value = 'O';
            else if (gboard->board[i][j] == C_VALUE) // Client 2
                value = 'X';
            else
                value = ' ';
            printf("| %c ", value);
        }
        printf("|");
        printf("\n+-----------+\n");
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

