#include "worker.h"
#include "scheduler.h"

#include <stdio.h>
#include <time.h>

static unsigned long elapsed_microseconds(
    const struct timespec *start,
    const struct timespec *end
)
{
    unsigned long seconds;
    long nanoseconds;

    seconds = (unsigned long)(end->tv_sec - start->tv_sec);
    nanoseconds = end->tv_nsec - start->tv_nsec;

    if (nanoseconds < 0) {
        seconds--;
        nanoseconds += 1000000000L;
    }

    return (seconds * 1000000UL) +
           ((unsigned long)nanoseconds / 1000UL);
}

void *worker_thread(void *argument)
{
    TaskForge *engine;
    Task *task;

    struct timespec start;
    struct timespec end;

    int result;

    TaskCleanupFunction cleanup;
    void *cleanup_argument;

    engine = (TaskForge *)argument;

    while (1) {

        pthread_mutex_lock(&engine->mutex);

        while (queue_is_empty(&engine->queue) &&
               !engine->shutting_down) {

            pthread_cond_wait(
                &engine->task_available,
                &engine->mutex
            );
        }

        if (engine->shutting_down &&
            queue_is_empty(&engine->queue)) {

            pthread_mutex_unlock(&engine->mutex);

            break;
        }

        task = queue_pop(&engine->queue);

        if (task == NULL) {

            pthread_mutex_unlock(&engine->mutex);

            continue;
        }

        if (task->cancel_requested) {

            task->state = TASK_CANCELLED;

            stats_task_cancelled(
                &engine->stats
            );

            pthread_cond_broadcast(
                &task->completed
            );

            pthread_mutex_unlock(&engine->mutex);

            continue;
        }

        task->state = TASK_RUNNING;

        engine->active_tasks++;

        clock_gettime(
            CLOCK_MONOTONIC,
            &task->started_at
        );

        pthread_mutex_unlock(&engine->mutex);

        clock_gettime(
            CLOCK_MONOTONIC,
            &start
        );

        result = task->function(
            task->argument
        );

        clock_gettime(
            CLOCK_MONOTONIC,
            &end
        );

        pthread_mutex_lock(&engine->mutex);

        cleanup = task->cleanup;
        cleanup_argument = task->argument;

        task->cleanup = NULL;
        task->argument = NULL;

        task->result = result;

        task->finished_at = end;

        if (result == 0) {

            task->state = TASK_COMPLETED;

            stats_task_completed(
                &engine->stats,
                elapsed_microseconds(
                    &start,
                    &end
                )
            );

        } else {

            task->state = TASK_FAILED;

            stats_task_failed(
                &engine->stats,
                elapsed_microseconds(
                    &start,
                    &end
                )
            );
        }

        engine->active_tasks--;

        pthread_cond_broadcast(
            &task->completed
        );

        if (engine->active_tasks == 0 &&
            queue_is_empty(&engine->queue)) {

            pthread_cond_broadcast(
                &engine->all_tasks_done
            );
        }

        pthread_mutex_unlock(&engine->mutex);

        if (cleanup != NULL) {

            cleanup(cleanup_argument);
        }
    }

    return NULL;
}
