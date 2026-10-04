#define _POSIX_C_SOURCE 200809L

#include "taskforge.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int task_number;
    int duration;
} TestTaskData;

static int test_task(void *argument)
{
    TestTaskData *data;

    data = (TestTaskData *)argument;

    printf("[TASK %d] Started | %d sec\n",
           data->task_number,
           data->duration);

    sleep(data->duration);

    printf("[TASK %d] Completed\n",
           data->task_number);

    return 0;
}

static void test_task_cleanup(void *argument)
{
    (void)argument;
}

static void short_delay(void)
{
    struct timespec delay;

    delay.tv_sec = 0;
    delay.tv_nsec = 200000000L;

    nanosleep(&delay, NULL);
}

int main(void)
{
    TaskForge engine;

    TestTaskData task_data[5];
    unsigned long task_ids[5];

    int i;

    printf("\n");
    printf("========================================\n");
    printf("       TASKFORGE PRIORITY TEST\n");
    printf("========================================\n");

    if (taskforge_init(&engine, 1) != 0) {
        printf("Failed to initialize TaskForge.\n");
        return 1;
    }

    task_data[0].task_number = 1;
    task_data[0].duration = 2;

    task_ids[0] = taskforge_submit(
        &engine,
        test_task,
        &task_data[0],
        test_task_cleanup,
        PRIORITY_LOW
    );

    if (task_ids[0] == 0) {
        printf("Failed to submit Task 1.\n");
        taskforge_shutdown(&engine);
        return 1;
    }

    short_delay();

    task_data[1].task_number = 2;
    task_data[1].duration = 1;

    task_ids[1] = taskforge_submit(
        &engine,
        test_task,
        &task_data[1],
        test_task_cleanup,
        PRIORITY_LOW
    );

    task_data[2].task_number = 3;
    task_data[2].duration = 1;

    task_ids[2] = taskforge_submit(
        &engine,
        test_task,
        &task_data[2],
        test_task_cleanup,
        PRIORITY_CRITICAL
    );

    task_data[3].task_number = 4;
    task_data[3].duration = 1;

    task_ids[3] = taskforge_submit(
        &engine,
        test_task,
        &task_data[3],
        test_task_cleanup,
        PRIORITY_HIGH
    );

    task_data[4].task_number = 5;
    task_data[4].duration = 1;

    task_ids[4] = taskforge_submit(
        &engine,
        test_task,
        &task_data[4],
        test_task_cleanup,
        PRIORITY_NORMAL
    );

    printf("\nSubmitted tasks:\n");
    printf("Task 1 : %lu (LOW)\n", task_ids[0]);
    printf("Task 2 : %lu (LOW)\n", task_ids[1]);
    printf("Task 3 : %lu (CRITICAL)\n", task_ids[2]);
    printf("Task 4 : %lu (HIGH)\n", task_ids[3]);
    printf("Task 5 : %lu (NORMAL)\n", task_ids[4]);

    printf("\nExpected order after Task 1:\n");
    printf("Task 3 (CRITICAL)\n");
    printf("Task 4 (HIGH)\n");
    printf("Task 5 (NORMAL)\n");
    printf("Task 2 (LOW)\n");

    printf("\nWaiting for all tasks...\n");

    taskforge_wait_all(&engine);

    printf("\nAll tasks completed.\n");

    taskforge_print_stats(&engine);

    for (i = 0; i < 5; i++) {
        if (taskforge_wait(&engine, task_ids[i]) != 0) {
            printf("Failed to wait for Task %d.\n", i + 1);
            taskforge_shutdown(&engine);
            return 1;
        }
    }

    if (engine.stats.completed == 5 &&
        engine.stats.failed == 0 &&
        engine.stats.cancelled == 0) {

        printf("\nPRIORITY TEST PASSED\n");

    } else {

        printf("\nPRIORITY TEST FAILED\n");

        taskforge_shutdown(&engine);

        return 1;
    }

    taskforge_shutdown(&engine);

    return 0;
}
