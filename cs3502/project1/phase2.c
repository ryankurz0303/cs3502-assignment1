#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 4
#define TRANSACTIONS 1000

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account account;

void *teller(void *arg) {
    int teller_id = *(int *)arg;

    for (int i = 0; i < TRANSACTIONS; i++) {
        pthread_mutex_lock(&account.lock);

        double old_balance = account.balance;

        usleep(10);

        account.balance = old_balance + 1.0;
        account.transaction_count++;

        pthread_mutex_unlock(&account.lock);

        if (i % 250 == 0) {
            printf(
                "Teller %d completed transaction %d\n",
                teller_id,
                i
            );
        }
    }

    printf("Teller %d finished\n", teller_id);

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    account.account_id = 1;
    account.balance = 1000.0;
    account.transaction_count = 0;

    if (pthread_mutex_init(&account.lock, NULL) != 0) {
        printf("Error initializing mutex\n");
        return 1;
    }

    double expected_balance =
        1000.0 + (NUM_THREADS * TRANSACTIONS);

    printf("Phase 2: Mutex Protection\n");
    printf("Initial balance: %.2f\n", account.balance);
    printf("Expected final balance: %.2f\n\n", expected_balance);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;

        if (pthread_create(
                &threads[i],
                NULL,
                teller,
                &thread_ids[i]) != 0) {
            printf("Error creating thread %d\n", i + 1);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nExpected balance: %.2f\n", expected_balance);
    printf("Actual balance: %.2f\n", account.balance);

    printf(
        "Expected transaction count: %d\n",
        NUM_THREADS * TRANSACTIONS
    );

    printf(
        "Actual transaction count: %d\n",
        account.transaction_count
    );

    if (account.balance == expected_balance &&
        account.transaction_count ==
            NUM_THREADS * TRANSACTIONS) {
        printf("\nMutex protection worked. Results are correct.\n");
    } else {
        printf("\nUnexpected result. Synchronization failed.\n");
    }

    pthread_mutex_destroy(&account.lock);

    return 0;
}
