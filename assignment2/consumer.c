// Ryan Kurz
// CS 3502 Assignment 2
// consumer.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#include "buffer.h"

#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define SEM_MUTEX "/sem_mutex"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./consumer <id> <num_items>\n");
        return 1;
    }

    int consumer_id = atoi(argv[1]);
    int num_items = atoi(argv[2]);

    int shm_id = shmget(SHM_KEY, sizeof(shared_buffer_t), 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        return 1;
    }

    shared_buffer_t *buffer = (shared_buffer_t *) shmat(shm_id, NULL, 0);
    if (buffer == (void *) -1) {
        perror("shmat failed");
        return 1;
    }

    sem_t *empty = sem_open(SEM_EMPTY, 0);
    sem_t *full = sem_open(SEM_FULL, 0);
    sem_t *mutex = sem_open(SEM_MUTEX, 0);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open failed");
        shmdt(buffer);
        return 1;
    }

    for (int i = 0; i < num_items; i++) {
        sem_wait(full);
        sem_wait(mutex);

        item_t item = buffer->buffer[buffer->tail];

        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
        buffer->count--;

        sem_post(mutex);
        sem_post(empty);

        printf("Consumer %d: Consumed value %d from Producer %d\n",
               consumer_id, item.value, item.producer_id);
    }

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    shmdt(buffer);

    return 0;
}
