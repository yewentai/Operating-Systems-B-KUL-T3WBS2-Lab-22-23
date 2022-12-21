#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>

#define SIZE 25 
#define READ_END 0 
#define WRITE_END 1

int main(void) {
    char wmsg[SIZE] = "Hi There";
    char rmsg[SIZE];
    pid_t pid;
    int fd[2];
    // create the pipe
    if (pipe(fd) == -1) {
        perror("pipe()");
        exit(1);
    }
    // fork the child
    pid = fork();
    if (pid < 0) { // fork error 
        perror("fork()");
        exit(1);
    }
    if (pid > 0) { // parent process
        close(fd[READ_END]);
        write(fd[WRITE_END], wmsg, SIZE);
        close(fd[WRITE_END]);
    }
    else { // child process
        close(fd[WRITE_END]);
        int len = read(fd[READ_END], rmsg, SIZE);
        for (int i = 0; i < len; i++) {
            if (islower(rmsg[i]) > 0)
                printf("%c", toupper(rmsg[i]));
            else
                printf("%c", tolower(rmsg[i]));
        }
        printf("\n");
        close(fd[READ_END]);
    }
    exit(0);
}