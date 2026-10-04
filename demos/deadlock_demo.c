#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex_a;
pthread_mutex_t mutex_b;

void get_future_time(
    struct timespec *timeout,
    int seconds
)
{
    if (clock_gettime(
            CLOCK_REALTIME,
            timeout
        ) == -1) {

        perror("clock_gettime");
        exit(1);
    }

    timeout->tv_sec += seconds;
}

void *thread_one(void *argument)
{
    struct timespec timeout;
    int result;

    (void)argument;

    printf("[THREAD 1] Locking mutex A...\n");

    pthread_mutex_lock(&mutex_a);

    printf("[THREAD 1] Mutex A acquired.\n");

    sleep(1);

    printf("[THREAD 1] Trying to lock mutex B...\n");

    get_future_time(&timeout, 2);

    result = pthread_mutex_timedlock(
        &mutex_b,
        &timeout
    );

    if (result == ETIMEDOUT) {

        printf("[THREAD 1] Timeout while waiting for mutex B.\n");
        printf("[THREAD 1] Deadlock risk detected.\n");

    } else if (result == 0) {

        printf("[THREAD 1] Mutex B acquired.\n");

        pthread_mutex_unlock(&mutex_b);

    } else {

        printf("[THREAD 1] pthread_mutex_timedlock() failed: %d\n",
               result);
    }

    pthread_mutex_unlock(&mutex_a);

    printf("[THREAD 1] Mutex A released.\n");

    return NULL;
}

void *thread_two(void *argument)
{
    struct timespec timeout;
    int result;

    (void)argument;

    printf("[THREAD 2] Locking mutex B...\n");

    pthread_mutex_lock(&mutex_b);

    printf("[THREAD 2] Mutex B acquired.\n");

    sleep(1);

    printf("[THREAD 2] Trying to lock mutex A...\n");

    get_future_time(&timeout, 2);

    result = pthread_mutex_timedlock(
        &mutex_a,
        &timeout
    );

    if (result == ETIMEDOUT) {

        printf("[THREAD 2] Timeout while waiting for mutex A.\n");
        printf("[THREAD 2] Deadlock risk detected.\n");

    } else if (result == 0) {

        printf("[THREAD 2] Mutex A acquired.\n");

        pthread_mutex_unlock(&mutex_a);

    } else {

        printf("[THREAD 2] pthread_mutex_timedlock() failed: %d\n",
               result);
    }

    pthread_mutex_unlock(&mutex_b);

    printf("[THREAD 2] Mutex B released.\n");

    return NULL;
}

int main(void)
{
    pthread_t thread_one_id;
    pthread_t thread_two_id;

    printf("========================================\n");
    printf("      TASKFORGE DEADLOCK DEMO\n");
    printf("========================================\n\n");

    printf("[1] Initializing Mutexes\n");
    printf("----------------------------------------\n");

    if (pthread_mutex_init(
            &mutex_a,
            NULL
        ) != 0) {

        perror("pthread_mutex_init");
        return 1;
    }

    if (pthread_mutex_init(
            &mutex_b,
            NULL
        ) != 0) {

        perror("pthread_mutex_init");
        pthread_mutex_destroy(&mutex_a);
        return 1;
    }

    printf("Mutex A initialized.\n");
    printf("Mutex B initialized.\n");

    printf("\n[2] Starting Threads\n");
    printf("----------------------------------------\n");

    if (pthread_create(
            &thread_one_id,
            NULL,
            thread_one,
            NULL
        ) != 0) {

        perror("pthread_create");
        return 1;
    }

    if (pthread_create(
            &thread_two_id,
            NULL,
            thread_two,
            NULL
        ) != 0) {

        perror("pthread_create");
        pthread_join(thread_one_id, NULL);
        return 1;
    }

    if (pthread_join(
            thread_one_id,
            NULL
        ) != 0) {

        perror("pthread_join");
        return 1;
    }

    if (pthread_join(
            thread_two_id,
            NULL
        ) != 0) {

        perror("pthread_join");
        return 1;
    }

    printf("\n[3] Deadlock Analysis\n");
    printf("----------------------------------------\n");

    printf("Thread 1 acquired A and requested B.\n");
    printf("Thread 2 acquired B and requested A.\n");
    printf("This creates a circular wait condition.\n");

    printf("\nTimed mutex acquisition prevented the program\n");
    printf("from waiting forever and exposed the deadlock risk.\n");

    printf("\n[4] Cleanup\n");
    printf("----------------------------------------\n");

    pthread_mutex_destroy(&mutex_a);
    pthread_mutex_destroy(&mutex_b);

    printf("Mutexes destroyed successfully.\n");

    printf("\nDeadlock detection demonstration completed.\n");

    return 0;
}
