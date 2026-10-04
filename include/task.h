#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <time.h>

typedef enum {
    TASK_QUEUED,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_CANCELLED
} TaskState;

typedef enum {
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_CRITICAL = 4
} TaskPriority;

typedef int (*TaskFunction)(void *arg);

typedef void (*TaskCleanupFunction)(void *arg);

typedef struct Task {
    unsigned long id;

    TaskFunction function;
    TaskCleanupFunction cleanup;

    void *argument;

    TaskPriority priority;
    TaskState state;

    int result;
    int cancel_requested;

    unsigned long sequence;

    struct timespec submitted_at;
    struct timespec started_at;
    struct timespec finished_at;

    pthread_cond_t completed;

    struct Task *next;
} Task;

const char *task_state_to_string(TaskState state);

const char *task_priority_to_string(
    TaskPriority priority
);

#endif
