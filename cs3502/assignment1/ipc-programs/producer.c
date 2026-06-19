#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;
long total_bytes = 0;
void handle_sigint(int sig) {
    shutdown_flag = 1;
}

void handle_sigusr1(int sig) {
    fprintf(stderr, "\nProducer stats: bytes written = %ld\n", total_bytes);
}

int main(int argc, char *argv[]) {
    char *filename = NULL;
    int buffer_size = 4096;
    int opt;
    FILE *input = stdin;
    struct sigaction sa_int, sa_usr1;
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);
    while ((opt = getopt(argc, argv, "f:b:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'b':
                buffer_size = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-f file] [-b size]\n", argv[0]);
                return 1;
        }
    }

    if (filename != NULL) {
        input = fopen(filename, "r");
        if (input == NULL) {
            perror("Could not open input file");
            return 1;
        }
    }

    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Could not allocate buffer\n");
        if (input != stdin) fclose(input);
        return 1;
    }

    size_t bytes_read;
    while (!shutdown_flag && (bytes_read = fread(buffer, 1, buffer_size, input)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
        fflush(stdout);
        total_bytes += bytes_read;
    }

    free(buffer);
    if (input != stdin) {
        fclose(input);
    }

    fprintf(stderr, "Producer finished. Total bytes: %ld\n", total_bytes);
    return 0;
}
