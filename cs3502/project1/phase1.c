#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 4
#define TRANSACTIONS 1000

double balance = 1000.0;
int transaction_count = 0;

void *teller(void *arg) {
    int teller_id = *(int *)arg;

    for (int i = 0; i < TRANSACTIONS; i++) {
        double old_balance = balance;

        // Makes the race condition easier to see
        usleep(10);

        balance = old_balance + 1.0;
        transaction_count++;

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

    double expected_balance =
        1000.0 + (NUM_THREADS * TRANSACTIONS);

    printf("Phase 1: Race Condition Demonstration\n");
    printf("Initial balance: %.2f\n", balance);
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
    printf("Actual balance: %.2f\n", balance);

    printf(
        "Expected transaction count: %d\n",
        NUM_THREADS * TRANSACTIONS
    );

    printf(
        "Actual transaction count: %d\n",
        transaction_count
    );

    if (balance != expected_balance) {
        printf("\nRace condition detected!\n");
    } else {
        printf("\nBalance happened to be correct. Run it again.\n");
    }

    return 0;
}
