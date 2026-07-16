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

int completed_transfers = 0;

void *unsafe_transfer(void *arg) {
    TransferData *data = (TransferData *)arg;

    printf(
        "Thread %d: attempting to transfer %.2f from account %d to account %d\n",
        data->thread_id,
        data->amount,
        data->from_account,
        data->to_account
    );

    printf(
        "Thread %d: trying to lock account %d\n",
        data->thread_id,
        data->from_account
    );

    pthread_mutex_lock(&accounts[data->from_account].lock);

    printf(
        "Thread %d: locked account %d\n",
        data->thread_id,
        data->from_account
    );

    sleep(1);

    printf(
        "Thread %d: waiting to lock account %d\n",
        data->thread_id,
        data->to_account
    );

    pthread_mutex_lock(&accounts[data->to_account].lock);

    accounts[data->from_account].balance -= data->amount;
    accounts[data->to_account].balance += data->amount;

    completed_transfers++;

    pthread_mutex_unlock(&accounts[data->to_account].lock);
    pthread_mutex_unlock(&accounts[data->from_account].lock);

    printf("Thread %d: transfer completed\n", data->thread_id);

    return NULL;
}

void *deadlock_monitor(void *arg) {
    (void)arg;

    sleep(5);

    if (completed_transfers == 0) {
        printf(
            "\nPossible deadlock detected: "
            "no transfers completed after 5 seconds.\n"
        );
        printf("Press Ctrl+C to stop the program.\n");
    }

    return NULL;
}

int main(void) {
    pthread_t thread1;
    pthread_t thread2;
    pthread_t monitor_thread;

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

    printf("Phase 3: Deadlock Demonstration\n");
    printf("Account 0 balance: %.2f\n", accounts[0].balance);
    printf("Account 1 balance: %.2f\n\n", accounts[1].balance);

    pthread_create(&thread1, NULL, unsafe_transfer, &transfer1);
    pthread_create(&thread2, NULL, unsafe_transfer, &transfer2);
    pthread_create(&monitor_thread, NULL, deadlock_monitor, NULL);

    pthread_join(monitor_thread, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
