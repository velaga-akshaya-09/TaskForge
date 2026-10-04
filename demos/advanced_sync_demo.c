#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define READER_COUNT 3

int shared_value = 100;

pthread_rwlock_t data_lock;

void *reader_worker(void *argument)
{
    int reader_id = *(int *)argument;

    printf("[READER %d] Waiting for read lock...\n",
           reader_id);

    if (pthread_rwlock_rdlock(&data_lock) != 0) {
        perror("pthread_rwlock_rdlock");
        return NULL;
    }

    printf("[READER %d] Read lock acquired.\n",
           reader_id);

    printf("[READER %d] Reading shared value: %d\n",
           reader_id,
           shared_value);

    sleep(1);

    printf("[READER %d] Releasing read lock.\n",
           reader_id);

    pthread_rwlock_unlock(&data_lock);

    return NULL;
}

void *writer_worker(void *argument)
{
    int writer_id = *(int *)argument;

    printf("[WRITER %d] Waiting for write lock...\n",
           writer_id);

    if (pthread_rwlock_wrlock(&data_lock) != 0) {
        perror("pthread_rwlock_wrlock");
        return NULL;
    }

    printf("[WRITER %d] Write lock acquired.\n",
           writer_id);

    shared_value += 50;

    printf("[WRITER %d] Modified shared value to: %d\n",
           writer_id,
           shared_value);

    sleep(1);

    printf("[WRITER %d] Releasing write lock.\n",
           writer_id);

    pthread_rwlock_unlock(&data_lock);

    return NULL;
}

int main(void)
{
    pthread_t readers[READER_COUNT];
    pthread_t writer;

    int reader_ids[READER_COUNT];
    int writer_id = 1;

    int i;

    printf("========================================\n");
    printf("   TASKFORGE ADVANCED SYNC DEMO\n");
    printf("========================================\n\n");

    printf("[1] Initializing Read-Write Lock\n");
    printf("----------------------------------------\n");

    if (pthread_rwlock_init(
            &data_lock,
            NULL
        ) != 0) {

        perror("pthread_rwlock_init");
        return 1;
    }

    printf("Read-write lock initialized.\n");
    printf("Initial shared value: %d\n",
           shared_value);

    /*
     * Start readers first.
     * Multiple readers can hold the read lock
     * simultaneously.
     */
    printf("\n[2] Starting Reader Threads\n");
    printf("----------------------------------------\n");

    for (i = 0; i < READER_COUNT; i++) {

        reader_ids[i] = i + 1;

        if (pthread_create(
                &readers[i],
                NULL,
                reader_worker,
                &reader_ids[i]
            ) != 0) {

            perror("pthread_create");
            pthread_rwlock_destroy(&data_lock);
            return 1;
        }
    }

    /*
     * Give readers time to acquire the read lock
     * before starting the writer.
     */
    sleep(1);

    printf("\n[3] Starting Writer Thread\n");
    printf("----------------------------------------\n");

    if (pthread_create(
            &writer,
            NULL,
            writer_worker,
            &writer_id
        ) != 0) {

        perror("pthread_create");

        for (i = 0; i < READER_COUNT; i++) {
            pthread_join(readers[i], NULL);
        }

        pthread_rwlock_destroy(&data_lock);

        return 1;
    }

    /*
     * Wait for readers.
     */
    for (i = 0; i < READER_COUNT; i++) {

        if (pthread_join(
                readers[i],
                NULL
            ) != 0) {

            perror("pthread_join");
            return 1;
        }
    }

    /*
     * Wait for writer.
     */
    if (pthread_join(
            writer,
            NULL
        ) != 0) {

        perror("pthread_join");
        return 1;
    }

    printf("\n[4] Synchronization Result\n");
    printf("----------------------------------------\n");

    printf("Final shared value: %d\n",
           shared_value);

    printf("\nRead-write lock behavior:\n");
    printf("  Multiple readers can read concurrently.\n");
    printf("  A writer obtains exclusive access.\n");
    printf("  Readers and writers are synchronized.\n");

    printf("\n[5] Cleanup\n");
    printf("----------------------------------------\n");

    if (pthread_rwlock_destroy(
            &data_lock
        ) != 0) {

        perror("pthread_rwlock_destroy");
        return 1;
    }

    printf("Read-write lock destroyed successfully.\n");

    printf("\nAdvanced synchronization demonstration completed.\n");

    return 0;
}
