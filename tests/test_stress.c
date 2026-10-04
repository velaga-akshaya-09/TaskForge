#define _POSIX_C_SOURCE 200809L

#include "taskforge.h"

#include <stdio.h>
#include <time.h>

#define TOTAL_TASKS 100

static int stress_task(void *argument)
{
    int *value;
    struct timespec delay;

    value = (int *)argument;

    delay.tv_sec = 0;
    delay.tv_nsec = 10000000L;

    nanosleep(&delay, NULL);

    return *value == 1 ? 0 : -1;
}

static void stress_task_cleanup(void *argument)
{
    (void)argument;
}

int main(void)
{
    TaskForge engine;

    unsigned long task_ids[TOTAL_TASKS];

    int task_values[TOTAL_TASKS];

    int i;

    printf("\n");
    printf("========================================\n");
    printf("          TASKFORGE STRESS TEST\n");
    printf("========================================\n");

    if (taskforge_init(&engine, 3) != 0) {

        printf("Failed to initialize TaskForge.\n");

        return 1;
    }

    printf(
        "Submitting %d tasks...\n",
        TOTAL_TASKS
    );

    for (i = 0; i < TOTAL_TASKS; i++) {

        task_values[i] = 1;

        task_ids[i] = taskforge_submit(
            &engine,
            stress_task,
            &task_values[i],
            stress_task_cleanup,
            PRIORITY_NORMAL
        );

        if (task_ids[i] == 0) {

            printf(
                "Failed to submit task %d.\n",
                i + 1
            );

            taskforge_shutdown(&engine);

            return 1;
        }
    }

    printf(
        "All %d tasks submitted successfully.\n",
        TOTAL_TASKS
    );

    printf("Waiting for all tasks...\n");

    taskforge_wait_all(&engine);

    printf("All tasks completed.\n");

    taskforge_print_stats(&engine);

    if (engine.stats.completed == TOTAL_TASKS &&
        engine.stats.failed == 0 &&
        engine.stats.cancelled == 0) {

        printf("\n");
        printf("STRESS TEST PASSED\n");

    } else {

        printf("\n");
        printf("STRESS TEST FAILED\n");

        taskforge_shutdown(&engine);

        return 1;
    }

    taskforge_shutdown(&engine);

    return 0;
}
