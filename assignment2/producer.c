// Ryan Kurz
// CS 3502 Assignment 2
// producer.c

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
        printf("Usage: ./producer <id> <num_items>\n");
        return 1;
    }

    int producer_id = atoi(argv[1]);
    int num_items = atoi(argv[2]);

    int shm_id = shmget(SHM_KEY, sizeof(shared_buffer_t), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        return 1;
    }

    shared_buffer_t *buffer = (shared_buffer_t *) shmat(shm_id, NULL, 0);
    if (buffer == (void *) -1) {
        perror("shmat failed");
        return 1;
    }

    sem_t *empty = sem_open(SEM_EMPTY, O_CREAT, 0644, BUFFER_SIZE);
    sem_t *full = sem_open(SEM_FULL, O_CREAT, 0644, 0);
    sem_t *mutex = sem_open(SEM_MUTEX, O_CREAT, 0644, 1);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open failed");
        shmdt(buffer);
        return 1;
    }

    
    if (buffer->count < 0 || buffer->count > BUFFER_SIZE) {
        buffer->head = 0;
        buffer->tail = 0;
        buffer->count = 0;
    }

    for (int i = 0; i < num_items; i++) {
        int value = producer_id * 1000 + i;

        sem_wait(empty);
        sem_wait(mutex);

        buffer->buffer[buffer->head].value = value;
        buffer->buffer[buffer->head].producer_id = producer_id;

        buffer->head = (buffer->head + 1) % BUFFER_SIZE;
        buffer->count++;

        sem_post(mutex);
        sem_post(full);

        printf("Producer %d: Produced value %d\n", producer_id, value);
    }

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    shmdt(buffer);

    return 0;
}
