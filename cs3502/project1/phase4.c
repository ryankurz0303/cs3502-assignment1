#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

typedef struct {
    int thread_id;
    int from_account;
    int to_account;
    double amount;
} TransferData;

Account accounts[NUM_ACCOUNTS];

void *safe_transfer(void *arg) {
    TransferData *data = (TransferData *)arg;

    int first_lock;
    int second_lock;

    /*
     * Always lock the lower-numbered account first.
     * This prevents circular wait and therefore prevents deadlock.
     */
    if (data->from_account < data->to_account) {
        first_lock = data->from_account;
        second_lock = data->to_account;
    } else {
        first_lock = data->to_account;
        second_lock = data->from_account;
    }

    printf(
        "Thread %d: transferring %.2f from account %d to account %d\n",
        data->thread_id,
        data->amount,
        data->from_account,
        data->to_account
    );

    printf(
        "Thread %d: locking account %d first\n",
        data->thread_id,
        first_lock
    );

    pthread_mutex_lock(&accounts[first_lock].lock);

    printf(
        "Thread %d: locked account %d\n",
        data->thread_id,
        first_lock
    );

    sleep(1);

    printf(
        "Thread %d: locking account %d second\n",
        data->thread_id,
        second_lock
    );

    pthread_mutex_lock(&accounts[second_lock].lock);

    printf(
        "Thread %d: locked account %d\n",
        data->thread_id,
        second_lock
    );

    if (accounts[data->from_account].balance >= data->amount) {
        accounts[data->from_account].balance -= data->amount;
        accounts[data->to_account].balance += data->amount;

        printf(
            "Thread %d: transfer completed successfully\n",
            data->thread_id
        );
    } else {
        printf(
            "Thread %d: transfer failed due to insufficient funds\n",
            data->thread_id
        );
    }

    pthread_mutex_unlock(&accounts[second_lock].lock);
    pthread_mutex_unlock(&accounts[first_lock].lock);

    printf(
        "Thread %d: released both account locks\n",
        data->thread_id
    );

    return NULL;
}

int main(void) {
    pthread_t thread1;
    pthread_t thread2;

    TransferData transfer1 = {1, 0, 1, 100.0};
    TransferData transfer2 = {2, 1, 0, 50.0};

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = 1000.0;

        if (pthread_mutex_init(&accounts[i].lock, NULL) != 0) {
            printf("Error initializing mutex for account %d\n", i);
            return 1;
        }
    }

    printf("Phase 4: Deadlock Prevention Using Lock Ordering\n");
    printf("Initial account 0 balance: %.2f\n", accounts[0].balance);
    printf("Initial account 1 balance: %.2f\n\n", accounts[1].balance);

    if (pthread_create(
            &thread1,
            NULL,
            safe_transfer,
            &transfer1) != 0) {
        printf("Error creating thread 1\n");
        return 1;
    }

    if (pthread_create(
            &thread2,
            NULL,
            safe_transfer,
            &transfer2) != 0) {
        printf("Error creating thread 2\n");
        return 1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nBoth transfers completed without deadlock.\n");
    printf("Final account 0 balance: %.2f\n", accounts[0].balance);
    printf("Final account 1 balance: %.2f\n", accounts[1].balance);
    printf(
        "Total balance: %.2f\n",
        accounts[0].balance + accounts[1].balance
    );

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
