#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[100];
    char *message = "Hello from parent!";
    int n;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return 1;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0) {
        close(pipefd[1]);
        n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Child received: %s\n", buffer);
        }

        close(pipefd[0]);
    } else {
    
        close(pipefd[0]);

        if (write(pipefd[1], message, strlen(message) + 1) == -1) {
            perror("write failed");
            close(pipefd[1]);
            wait(NULL);
            return 1;
        }
        printf("Parent sent: %s\n", message);

        close(pipefd[1]);
        wait(NULL); 
    }

    return 0;
}

