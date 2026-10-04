#include "taskforge.h"
#include "worker.h"
#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static Task *find_task(
    TaskForge *engine,
    unsigned long task_id
)
{
    TaskNode *current;

    current = engine->task_registry;

    while (current != NULL) {

        if (current->task->id == task_id) {
            return current->task;
        }

        current = current->next;
    }

    return NULL;
}

static int add_task_to_registry(
    TaskForge *engine,
    Task *task
)
{
    TaskNode *node;

    node = malloc(sizeof(TaskNode));

    if (node == NULL) {
        return -1;
    }

    node->task = task;
    node->next = engine->task_registry;

    engine->task_registry = node;

    return 0;
}

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

int taskforge_init(
    TaskForge *engine,
    unsigned int worker_count
)
{
    unsigned int i;

    if (engine == NULL) {
        return -1;
    }

    if (worker_count == 0 ||
        worker_count > TASKFORGE_MAX_WORKERS) {
        return -1;
    }

    engine->workers = NULL;
    engine->worker_count = worker_count;

    engine->task_registry = NULL;

    engine->accepting = 1;
    engine->shutting_down = 0;

    engine->next_task_id = 1;
    engine->next_sequence = 1;
    engine->active_tasks = 0;

    queue_init(&engine->queue);
    stats_init(&engine->stats);

    if (pthread_mutex_init(&engine->mutex, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(
            &engine->task_available,
            NULL
        ) != 0) {

        pthread_mutex_destroy(&engine->mutex);
        return -1;
    }

    if (pthread_cond_init(
            &engine->all_tasks_done,
            NULL
        ) != 0) {

        pthread_cond_destroy(&engine->task_available);
        pthread_mutex_destroy(&engine->mutex);
        return -1;
    }

    engine->workers = malloc(
        sizeof(pthread_t) * worker_count
    );

    if (engine->workers == NULL) {

        pthread_cond_destroy(
            &engine->all_tasks_done
        );

        pthread_cond_destroy(
            &engine->task_available
        );

        pthread_mutex_destroy(
            &engine->mutex
        );

        return -1;
    }

    for (i = 0; i < worker_count; i++) {

        if (pthread_create(
                &engine->workers[i],
                NULL,
                worker_thread,
                engine
            ) != 0) {

            engine->shutting_down = 1;

            pthread_cond_broadcast(
                &engine->task_available
            );

            while (i > 0) {
                i--;

                pthread_join(
                    engine->workers[i],
                    NULL
                );
            }

            free(engine->workers);

            pthread_cond_destroy(
                &engine->all_tasks_done
            );

            pthread_cond_destroy(
                &engine->task_available
            );

            pthread_mutex_destroy(
                &engine->mutex
            );

            return -1;
        }
    }

    return 0;
}

unsigned long taskforge_submit(
    TaskForge *engine,
    TaskFunction function,
    void *argument,
    TaskCleanupFunction cleanup,
    TaskPriority priority
)
{
    Task *task;
    unsigned long task_id;

    if (engine == NULL ||
        function == NULL) {
        return 0;
    }

    task = malloc(sizeof(Task));

    if (task == NULL) {
        return 0;
    }

    task->function = function;
    task->argument = argument;
    task->cleanup = cleanup;
    task->priority = priority;

    task->state = TASK_QUEUED;
    task->result = 0;
    task->cancel_requested = 0;
    task->next = NULL;

    pthread_cond_init(
        &task->completed,
        NULL
    );

    clock_gettime(
        CLOCK_MONOTONIC,
        &task->submitted_at
    );

    pthread_mutex_lock(&engine->mutex);

    if (!engine->accepting ||
        engine->shutting_down) {

        pthread_mutex_unlock(&engine->mutex);

        pthread_cond_destroy(&task->completed);
        free(task);

        return 0;
    }

    task_id = engine->next_task_id++;

    task->id = task_id;
    task->sequence = engine->next_sequence++;

    if (add_task_to_registry(
            engine,
            task
        ) != 0) {

        pthread_mutex_unlock(&engine->mutex);

        pthread_cond_destroy(&task->completed);
        free(task);

        return 0;
    }

    scheduler_insert(
        &engine->queue,
        task
    );

    stats_task_submitted(
        &engine->stats,
        queue_size(&engine->queue)
    );

    pthread_cond_signal(
        &engine->task_available
    );

    pthread_mutex_unlock(&engine->mutex);

    return task_id;
}

int taskforge_wait(
    TaskForge *engine,
    unsigned long task_id
)
{
    Task *task;

    if (engine == NULL) {
        return -1;
    }

    pthread_mutex_lock(&engine->mutex);

    task = find_task(
        engine,
        task_id
    );

    if (task == NULL) {

        pthread_mutex_unlock(&engine->mutex);
        return -1;
    }

    while (task->state == TASK_QUEUED ||
           task->state == TASK_RUNNING) {

        pthread_cond_wait(
            &task->completed,
            &engine->mutex
        );
    }

    pthread_mutex_unlock(&engine->mutex);

    return 0;
}

int taskforge_cancel(
    TaskForge *engine,
    unsigned long task_id
)
{
    Task *task;

    if (engine == NULL) {
        return -1;
    }

    pthread_mutex_lock(&engine->mutex);

    task = find_task(
        engine,
        task_id
    );

    if (task == NULL) {

        pthread_mutex_unlock(&engine->mutex);
        return -1;
    }

    if (task->state != TASK_QUEUED) {

        pthread_mutex_unlock(&engine->mutex);
        return 0;
    }

    if (!queue_remove(
            &engine->queue,
            task
        )) {

        pthread_mutex_unlock(&engine->mutex);
        return -1;
    }

    task->cancel_requested = 1;
    task->state = TASK_CANCELLED;

    if (task->cleanup != NULL) {
        task->cleanup(task->argument);
        task->argument = NULL;
    }

    stats_task_cancelled(
        &engine->stats
    );

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

    return 1;
}

void taskforge_wait_all(
    TaskForge *engine
)
{
    if (engine == NULL) {
        return;
    }

    pthread_mutex_lock(&engine->mutex);

    while (engine->active_tasks > 0 ||
           !queue_is_empty(&engine->queue)) {

        pthread_cond_wait(
            &engine->all_tasks_done,
            &engine->mutex
        );
    }

    pthread_mutex_unlock(&engine->mutex);
}

void taskforge_print_task(
    TaskForge *engine,
    unsigned long task_id
)
{
    Task *task;
    unsigned long execution_time;

    if (engine == NULL) {
        return;
    }

    pthread_mutex_lock(&engine->mutex);

    task = find_task(
        engine,
        task_id
    );

    if (task == NULL) {

        printf(
            "Task %lu not found.\n",
            task_id
        );

        pthread_mutex_unlock(&engine->mutex);
        return;
    }

    printf("\n");
    printf("Task Information\n");
    printf("----------------\n");
    printf("ID        : %lu\n", task->id);
    printf("Priority  : %s\n",
           task_priority_to_string(task->priority));
    printf("State     : %s\n",
           task_state_to_string(task->state));
    printf("Result    : %d\n", task->result);
    printf("Sequence  : %lu\n", task->sequence);

    if (task->state == TASK_COMPLETED ||
        task->state == TASK_FAILED) {

        execution_time = elapsed_microseconds(
            &task->started_at,
            &task->finished_at
        );

        printf(
            "Execution : %lu us\n",
            execution_time
        );
    }

    printf("\n");

    pthread_mutex_unlock(&engine->mutex);
}

void taskforge_print_stats(
    TaskForge *engine
)
{
    double average;

    if (engine == NULL) {
        return;
    }

    pthread_mutex_lock(&engine->mutex);

    average = stats_average_execution_time(
        &engine->stats
    );

    printf("\n");
    printf("TaskForge Statistics\n");
    printf("--------------------\n");
    printf("Submitted       : %lu\n",
           engine->stats.submitted);
    printf("Completed       : %lu\n",
           engine->stats.completed);
    printf("Failed          : %lu\n",
           engine->stats.failed);
    printf("Cancelled       : %lu\n",
           engine->stats.cancelled);
    printf("Queue Peak      : %lu\n",
           engine->stats.peak_queue_size);
    printf("Average Time    : %.2f us\n",
           average);
    printf("Maximum Time    : %lu us\n",
           engine->stats.max_execution_time_us);
    printf("\n");

    pthread_mutex_unlock(&engine->mutex);
}

void taskforge_shutdown(
    TaskForge *engine
)
{
    unsigned int i;
    TaskNode *current;
    TaskNode *next;

    if (engine == NULL) {
        return;
    }

    pthread_mutex_lock(&engine->mutex);

    engine->accepting = 0;
    engine->shutting_down = 1;

    pthread_cond_broadcast(
        &engine->task_available
    );

    pthread_mutex_unlock(&engine->mutex);

    for (i = 0; i < engine->worker_count; i++) {

        pthread_join(
            engine->workers[i],
            NULL
        );
    }

    free(engine->workers);
    engine->workers = NULL;

    current = engine->task_registry;

    while (current != NULL) {

        next = current->next;

        if (current->task->cleanup != NULL &&
            current->task->argument != NULL) {

            current->task->cleanup(
                current->task->argument
            );

            current->task->argument = NULL;
        }

        pthread_cond_destroy(
            &current->task->completed
        );

        free(current->task);
        free(current);

        current = next;
    }

    engine->task_registry = NULL;

    pthread_cond_destroy(
        &engine->all_tasks_done
    );

    pthread_cond_destroy(
        &engine->task_available
    );

    pthread_mutex_destroy(
        &engine->mutex
    );
}
