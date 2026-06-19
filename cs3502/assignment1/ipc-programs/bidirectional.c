#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int to_child[2];
    int to_parent[2];
    pid_t pid;
    int n;

    char parent_msg[] = "Hello child, this is the parent.";
    char child_msg[] = "Hello parent, message received.";
    char msg_box[100];

    if (pipe(to_child) == -1) {
        perror("pipe failed");
        return 1;
    }

    if (pipe(to_parent) == -1) {
        perror("pipe failed");
        close(to_child[0]);
        close(to_child[1]);
        return 1;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        close(to_child[0]);
        close(to_child[1]);
        close(to_parent[0]);
        close(to_parent[1]);
        return 1;
    }

    if (pid == 0) {
        close(to_child[1]);
        close(to_parent[0]);

        n = read(to_child[0], msg_box, sizeof(msg_box) - 1);

        if (n > 0) {
            msg_box[n] = '\0';
            printf("Child received: %s\n", msg_box);
        } else {
            printf("Child received no message.\n");
        }

        if (write(to_parent[1], child_msg, strlen(child_msg) + 1) == -1) {
            perror("write failed");
            close(to_child[0]);
            close(to_parent[1]);
            return 1;
        }

        printf("Child sent response.\n");

        close(to_child[0]);
        close(to_parent[1]);
    } else {
        close(to_child[0]);
        close(to_parent[1]);

        if (write(to_child[1], parent_msg, strlen(parent_msg) + 1) == -1) {
            perror("write failed");
            close(to_child[1]);
            close(to_parent[0]);
            wait(NULL);
            return 1;
        }

        printf("Parent sent: %s\n", parent_msg);

        n = read(to_parent[0], msg_box, sizeof(msg_box) - 1);

        if (n > 0) {
            msg_box[n] = '\0';
            printf("Parent received: %s\n", msg_box);
        } else {
            printf("Parent received no response.\n");
        }

        close(to_child[1]);
        close(to_parent[0]);

        wait(NULL);
    }

    return 0;
}

