#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t shutdown_flag = 0;
long total_lines = 0;
long total_chars = 0;

void handle_sigint(int sig) {
    shutdown_flag = 1;
}

void handle_sigusr1(int sig) {
    fprintf(stderr, "\nConsumer stats: lines = %ld, chars = %ld\n", total_lines, total_chars);
}

int main(int argc, char *argv[]) {
    int max_lines = -1;
    int verbose = 0;
    int opt;
    char line[4096];
    struct sigaction sa_int, sa_usr1;

    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    while ((opt = getopt(argc, argv, "n:v")) != -1) {
        switch (opt) {
            case 'n':
                max_lines = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n max] [-v]\n", argv[0]);
                return 1;
        }
    }

    while (!shutdown_flag && fgets(line, sizeof(line), stdin) != NULL) {
        total_lines++;
        total_chars += strlen(line);

        if (verbose) {
            printf("%s", line);
        }

        if (max_lines > 0 && total_lines >= max_lines) {
            break;
        }
    }

    fprintf(stderr, "Consumer finished.\n");
    fprintf(stderr, "Lines: %ld\n", total_lines);
    fprintf(stderr, "Characters: %ld\n", total_chars);

    return 0;
}
