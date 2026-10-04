#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT 4
#define INCREMENTS 100000

long shared_counter = 0;

pthread_mutex_t counter_mutex;

void *unsafe_worker(void *argument)
{
    int i;

    (void)argument;

    for (i = 0; i < INCREMENTS; i++) {
        shared_counter++;
    }

    return NULL;
}

void *safe_worker(void *argument)
{
    int i;

    (void)argument;

    for (i = 0; i < INCREMENTS; i++) {

        pthread_mutex_lock(&counter_mutex);

        shared_counter++;

        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];

    int i;

    long expected_value =
        (long)THREAD_COUNT * INCREMENTS;

    printf("========================================\n");
    printf("     TASKFORGE RACE / MUTEX DEMO\n");
    printf("========================================\n\n");

    /*
     * ------------------------------------------------
     * PART 1: UNSYNCHRONIZED ACCESS
     * ------------------------------------------------
     */

    printf("[1] Race Condition Demonstration\n");
    printf("----------------------------------------\n");

    shared_counter = 0;

    printf("Threads            : %d\n",
           THREAD_COUNT);

    printf("Increments/thread  : %d\n",
           INCREMENTS);

    printf("Expected counter   : %ld\n",
           expected_value);

    printf("Running without mutex...\n");

    for (i = 0; i < THREAD_COUNT; i++) {

        if (pthread_create(
                &threads[i],
                NULL,
                unsafe_worker,
                NULL
            ) != 0) {

            perror("pthread_create");
            return 1;
        }
    }

    for (i = 0; i < THREAD_COUNT; i++) {

        if (pthread_join(
                threads[i],
                NULL
            ) != 0) {

            perror("pthread_join");
            return 1;
        }
    }

    printf("Result without mutex: %ld\n",
           shared_counter);

    if (shared_counter != expected_value) {

        printf("Race condition detected: "
               "updates were lost.\n");

    } else {

        printf("This run produced the expected value.\n");
        printf("The race condition may not appear every run.\n");
    }

    /*
     * ------------------------------------------------
     * PART 2: MUTEX PROTECTION
     * ------------------------------------------------
     */

    printf("\n[2] Mutex Protection\n");
    printf("----------------------------------------\n");

    shared_counter = 0;

    if (pthread_mutex_init(
            &counter_mutex,
            NULL
        ) != 0) {

        perror("pthread_mutex_init");
        return 1;
    }

    printf("Running with mutex protection...\n");

    for (i = 0; i < THREAD_COUNT; i++) {

        if (pthread_create(
                &threads[i],
                NULL,
                safe_worker,
                NULL
            ) != 0) {

            perror("pthread_create");

            pthread_mutex_destroy(
                &counter_mutex
            );

            return 1;
        }
    }

    for (i = 0; i < THREAD_COUNT; i++) {

        if (pthread_join(
                threads[i],
                NULL
            ) != 0) {

            perror("pthread_join");

            pthread_mutex_destroy(
                &counter_mutex
            );

            return 1;
        }
    }

    printf("Result with mutex: %ld\n",
           shared_counter);

    if (shared_counter == expected_value) {

        printf("Mutex protected the shared resource correctly.\n");

    } else {

        printf("Unexpected result.\n");
    }

    if (pthread_mutex_destroy(
            &counter_mutex
        ) != 0) {

        perror("pthread_mutex_destroy");
        return 1;
    }

    printf("\n[3] Synchronization Summary\n");
    printf("----------------------------------------\n");

    printf("Without mutex:\n");
    printf("  Multiple threads access shared data simultaneously.\n");
    printf("  Concurrent updates can be lost.\n");

    printf("\nWith mutex:\n");
    printf("  pthread_mutex_lock() protects the critical section.\n");
    printf("  Only one thread updates the counter at a time.\n");

    printf("\nRace condition and mutex demonstration completed.\n");

    return 0;
}
