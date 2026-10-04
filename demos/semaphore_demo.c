#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_COUNT 4

sem_t resource_semaphore;

void *worker(void *argument)
{
    int thread_id = *(int *)argument;

    printf("[THREAD %d] Waiting for semaphore...\n",
           thread_id);

    if (sem_wait(&resource_semaphore) != 0) {
        perror("sem_wait");
        return NULL;
    }

    printf("[THREAD %d] Acquired semaphore.\n",
           thread_id);

    printf("[THREAD %d] Using shared resource...\n",
           thread_id);

    sleep(1);

    printf("[THREAD %d] Releasing semaphore.\n",
           thread_id);

    if (sem_post(&resource_semaphore) != 0) {
        perror("sem_post");
        return NULL;
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];
    int thread_ids[THREAD_COUNT];

    int i;

    printf("========================================\n");
    printf("      TASKFORGE SEMAPHORE DEMO\n");
    printf("========================================\n\n");

    printf("[1] Initializing Semaphore\n");
    printf("----------------------------------------\n");

    /*
     * Initial value = 2.
     *
     * This means at most two threads can
     * acquire the semaphore simultaneously.
     */
    if (sem_init(
            &resource_semaphore,
            0,
            2
        ) != 0) {

        perror("sem_init");
        return 1;
    }

    printf("Semaphore initialized.\n");
    printf("Maximum simultaneous resource users: 2\n");

    printf("\n[2] Creating Threads\n");
    printf("----------------------------------------\n");

    for (i = 0; i < THREAD_COUNT; i++) {

        thread_ids[i] = i + 1;

        if (pthread_create(
                &threads[i],
                NULL,
                worker,
                &thread_ids[i]
            ) != 0) {

            perror("pthread_create");
            sem_destroy(&resource_semaphore);
            return 1;
        }

        printf("Created thread %d.\n",
               thread_ids[i]);
    }

    printf("\n[3] Waiting for Threads\n");
    printf("----------------------------------------\n");

    for (i = 0; i < THREAD_COUNT; i++) {

        if (pthread_join(
                threads[i],
                NULL
            ) != 0) {

            perror("pthread_join");
            sem_destroy(&resource_semaphore);
            return 1;
        }
    }

    printf("All threads completed.\n");

    printf("\n[4] Destroying Semaphore\n");
    printf("----------------------------------------\n");

    if (sem_destroy(&resource_semaphore) != 0) {
        perror("sem_destroy");
        return 1;
    }

    printf("Semaphore destroyed successfully.\n");

    printf("\nSemaphore synchronization demonstration completed.\n");

    return 0;
}
