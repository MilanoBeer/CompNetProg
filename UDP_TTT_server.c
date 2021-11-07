#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

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
    int serv_sock;
    int send_len, rcv_len;
    socklen_t clnt_addr_sz;
    struct sockaddr_in serv_addr, clnt_addr;
    GAMEBOARD Board = {
        0,
    };
    int space_result;
    int row = 0, col = 0, randTemp, a, b;
    /* make random number */
    srand(time(NULL));

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (serv_sock == -1)
        error_handling("UDP socket creation error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    /* Start Server , print Board empty state */
    printf("Tic-Tac-Toe Server\n");
    draw_board(&Board);

    /* receive from Client, draw Board state */
    clnt_addr_sz = sizeof(clnt_addr);

    while (1)
    {
        if ((rcv_len = recvfrom(serv_sock, (void *)&Board, BUF_SIZE, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_sz)) == -1)
            error_handling("recv() error");
        draw_board(&Board);

        /* check available space in Board */
        if ((space_result = available_space(&Board)) == 0)
        {
            printf("No available space. Exit this program.Tic Tac Toe Server Close\n");
            break;
        }

        /*Server choose randomly, find Empty space */
        while (1)
        {
            row = rand() % BOARD_SIZE;
            col = rand() % BOARD_SIZE;
            if (Board.board[row][col] == 0)
                break;
        }
        Board.board[row][col] = 1;

        /* draw Board state */
        printf("Server choose: [%d, %d]\n", row, col);
        draw_board(&Board);

        /* send to Client Board-info */
        if ((send_len = sendto(serv_sock, (void *)&Board, sizeof(Board), 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz)) == -1)
            error_handling("sendto() error");
    }
    close(serv_sock);
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
            else if (gboard->board[i][j] == C_VALUE) // Client  2
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

