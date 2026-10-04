#ifndef TASKFORGE_H
#define TASKFORGE_H

#include <pthread.h>

#include "task.h"
#include "queue.h"
#include "stats.h"

#define TASKFORGE_MAX_WORKERS 64

typedef struct TaskNode {
    Task *task;
    struct TaskNode *next;
} TaskNode;

typedef struct TaskForge {

    pthread_t *workers;

    unsigned int worker_count;

    TaskQueue queue;

    TaskNode *task_registry;

    pthread_mutex_t mutex;

    pthread_cond_t task_available;
    pthread_cond_t all_tasks_done;

    int accepting;
    int shutting_down;

    unsigned long next_task_id;
    unsigned long next_sequence;

    unsigned long active_tasks;

    TaskForgeStats stats;

} TaskForge;

int taskforge_init(
    TaskForge *engine,
    unsigned int worker_count
);

unsigned long taskforge_submit(
    TaskForge *engine,
    TaskFunction function,
    void *argument,
    TaskCleanupFunction cleanup,
    TaskPriority priority
);

int taskforge_wait(
    TaskForge *engine,
    unsigned long task_id
);

int taskforge_cancel(
    TaskForge *engine,
    unsigned long task_id
);

void taskforge_wait_all(
    TaskForge *engine
);

void taskforge_print_task(
    TaskForge *engine,
    unsigned long task_id
);

void taskforge_print_stats(
    TaskForge *engine
);

void taskforge_shutdown(
    TaskForge *engine
);

#endif
