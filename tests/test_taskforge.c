#define _POSIX_C_SOURCE 200809L

#include "taskforge.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

static int test_task(void *argument)
{
    int duration;

    duration = *(int *)argument;

    sleep(duration);

    return 0;
}

static void test_task_cleanup(void *argument)
{
    (void)argument;
}

int main(void)
{
    TaskForge engine;

    int duration_long = 5;
    int duration_short = 1;

    unsigned long task1;
    unsigned long task2;
    unsigned long task3;
    unsigned long task4;

    printf("\n");
    printf("========================================\n");
    printf("       TASKFORGE PRIORITY TEST\n");
    printf("========================================\n");

    if (taskforge_init(&engine, 3) != 0) {

        printf("Failed to initialize TaskForge.\n");

        return 1;
    }

    task1 = taskforge_submit(
        &engine,
        test_task,
        &duration_long,
        test_task_cleanup,
        PRIORITY_LOW
    );

    task2 = taskforge_submit(
        &engine,
        test_task,
        &duration_long,
        test_task_cleanup,
        PRIORITY_LOW
    );

    task3 = taskforge_submit(
        &engine,
        test_task,
        &duration_long,
        test_task_cleanup,
        PRIORITY_LOW
    );

    {
        struct timespec delay;

        delay.tv_sec = 0;
        delay.tv_nsec = 200000000L;

        nanosleep(&delay, NULL);
    }

    task4 = taskforge_submit(
        &engine,
        test_task,
        &duration_short,
        test_task_cleanup,
        PRIORITY_CRITICAL
    );

    printf("\n");
    printf("Submitted tasks:\n");
    printf("Task 1 : %lu (LOW)\n", task1);
    printf("Task 2 : %lu (LOW)\n", task2);
    printf("Task 3 : %lu (LOW)\n", task3);
    printf("Task 4 : %lu (CRITICAL)\n", task4);

    {
        struct timespec delay;

        delay.tv_sec = 0;
        delay.tv_nsec = 100000000L;

        nanosleep(&delay, NULL);
    }

    printf("\n");
    printf("Checking Task 4...\n");

    taskforge_print_task(
        &engine,
        task4
    );

    printf("\n");
    printf("Waiting for all tasks...\n");

    taskforge_wait_all(&engine);

    printf("All tasks completed.\n");

    taskforge_print_stats(&engine);

    taskforge_shutdown(&engine);

    return 0;
}
