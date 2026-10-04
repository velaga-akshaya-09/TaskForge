#define _POSIX_C_SOURCE 200809L

#include "taskforge.h"

#include <stdio.h>
#include <unistd.h>

static int successful_task(void *argument)
{
    (void)argument;

    sleep(1);

    return 0;
}

static int failing_task(void *argument)
{
    (void)argument;

    sleep(1);

    return -1;
}

static void test_task_cleanup(void *argument)
{
    (void)argument;
}

int main(void)
{
    TaskForge engine;

    unsigned long task1;
    unsigned long task2;

    printf("\n");
    printf("========================================\n");
    printf("          TASKFORGE FAILURE TEST\n");
    printf("========================================\n");

    if (taskforge_init(&engine, 2) != 0) {

        printf("Failed to initialize TaskForge.\n");

        return 1;
    }

    task1 = taskforge_submit(
        &engine,
        successful_task,
        NULL,
        test_task_cleanup,
        PRIORITY_NORMAL
    );

    task2 = taskforge_submit(
        &engine,
        failing_task,
        NULL,
        test_task_cleanup,
        PRIORITY_NORMAL
    );

    printf("\nSubmitted tasks:\n");
    printf("Task 1 : %lu (successful)\n", task1);
    printf("Task 2 : %lu (failing)\n", task2);

    taskforge_wait(
        &engine,
        task1
    );

    taskforge_wait(
        &engine,
        task2
    );

    printf("\nTask 1 status:\n");

    taskforge_print_task(
        &engine,
        task1
    );

    printf("\nTask 2 status:\n");

    taskforge_print_task(
        &engine,
        task2
    );

    printf("\nFinal statistics:\n");

    taskforge_print_stats(
        &engine
    );

    taskforge_shutdown(
        &engine
    );

    return 0;
}
