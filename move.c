#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

// Argument
#define DFT_ARGC 3

// BUFFERING
#define BUF_SIZE 512

void error_handling(char* message);

int main(int argc, char *argv[])
{
        int src_fd, dest_fd;
        ssize_t src_size, dest_size;
        char buf[BUF_SIZE];

        // check argument
        if (argc != DFT_ARGC) {
            fprintf(stderr, "[ERROR] mymove Usage: %s %s %s\n", 
                argv[0],
                "src_file",
                "dest_file"
            );
            exit(EXIT_FAILURE);
        }

        // open source file read mode
        if ((src_fd = open(argv[1], O_RDONLY, 0644)) == -1) 
            error_handling("source file open() error!");

        // open destination file with write mode
        if ((dest_fd = open(argv[2], O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1) 
            error_handling("destination file open() error!");

        // read from src, and write to dest
        while ((src_size = read(src_fd, buf, BUF_SIZE)) != 0 && src_size != -1) 
            dest_size = write(dest_fd, buf, src_size);

        // close file descriptor
        close(src_fd);
        close(dest_fd);

        // remove src file
        if (remove(argv[1]) != 0)
            error_handling("source file remove() error!\n");

        // msg from mymove
        printf("move from %s to %s (bytes: %d) finished.\n",
            argv[1],
            argv[2],
            (int)dest_size
        );
        
        return 0;
}

void error_handling(char* message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}
