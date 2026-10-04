#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int task_number;
    int duration;
} CLITaskData;

static int cli_task(void *argument)
{
    CLITaskData *data;

    data = (CLITaskData *)argument;

    printf(
        "[TASK %d] Started | Worker executing for %d sec\n",
        data->task_number,
        data->duration
    );

    sleep(data->duration);

    printf(
        "[TASK %d] Completed\n",
        data->task_number
    );

    return 0;
}

static void cli_task_cleanup(void *argument)
{
    free(argument);
}

static void print_help(void)
{
    printf("\n");
    printf("TaskForge Commands\n");
    printf("------------------\n");
    printf("help              Show available commands\n");
    printf("submit            Submit a new task\n");
    printf("status <id>       Show task information\n");
    printf("cancel <id>       Cancel a queued task\n");
    printf("wait <id>         Wait for a specific task\n");
    printf("waitall           Wait for all tasks\n");
    printf("stats             Show engine statistics\n");
    printf("shutdown          Shutdown the engine\n");
    printf("exit              Shutdown and exit\n");
    printf("\n");
}

static TaskPriority read_priority(void)
{
    int choice;

    printf("\n");
    printf("Priority levels:\n");
    printf("1. LOW\n");
    printf("2. NORMAL\n");
    printf("3. HIGH\n");
    printf("4. CRITICAL\n");
    printf("Select priority: ");

    if (scanf("%d", &choice) != 1) {

        while (getchar() != '\n')
            ;

        return PRIORITY_NORMAL;
    }

    while (getchar() != '\n')
        ;

    if (choice < 1 || choice > 4) {
        return PRIORITY_NORMAL;
    }

    return (TaskPriority)choice;
}

static void submit_task(TaskForge *engine)
{
    CLITaskData *data;
    int duration;
    TaskPriority priority;
    unsigned long task_id;

    printf("\nTask duration in seconds: ");

    if (scanf("%d", &duration) != 1) {

        while (getchar() != '\n')
            ;

        printf("Invalid duration.\n");
        return;
    }

    while (getchar() != '\n')
        ;

    if (duration <= 0) {
        printf("Duration must be greater than zero.\n");
        return;
    }

    priority = read_priority();

    data = malloc(sizeof(CLITaskData));

    if (data == NULL) {
        printf("Failed to allocate task data.\n");
        return;
    }

    data->task_number = 0;
    data->duration = duration;

    task_id = taskforge_submit(
        engine,
        cli_task,
        data,
        cli_task_cleanup,
        priority
    );

    if (task_id == 0) {

        free(data);

        printf("Failed to submit task.\n");
        return;
    }

    data->task_number = (int)task_id;

    printf(
        "Task submitted successfully. ID = %lu\n",
        task_id
    );
}

void cli_run(TaskForge *engine)
{
    char command[128];
    unsigned long task_id;

    printf("\n");
    printf("========================================\n");
    printf("       TASKFORGE INTERACTIVE CLI\n");
    printf("========================================\n");

    print_help();

    while (1) {

        printf("TaskForge> ");

        if (fgets(
                command,
                sizeof(command),
                stdin
            ) == NULL) {

            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "help") == 0) {

            print_help();

        } else if (strcmp(command, "submit") == 0) {

            submit_task(engine);

        } else if (
            sscanf(command, "status %lu", &task_id) == 1
        ) {

            taskforge_print_task(
                engine,
                task_id
            );

        } else if (
            sscanf(command, "cancel %lu", &task_id) == 1
        ) {

            int result;

            result = taskforge_cancel(
                engine,
                task_id
            );

            if (result == 1) {
                printf(
                    "Task %lu cancelled.\n",
                    task_id
                );
            } else if (result == 0) {
                printf(
                    "Task %lu cannot be cancelled "
                    "(already running or finished).\n",
                    task_id
                );
            } else {
                printf(
                    "Task %lu not found.\n",
                    task_id
                );
            }

        } else if (
            sscanf(command, "wait %lu", &task_id) == 1
        ) {

            if (taskforge_wait(
                    engine,
                    task_id
                ) == 0) {

                printf(
                    "Task %lu finished.\n",
                    task_id
                );

            } else {

                printf(
                    "Task %lu not found.\n",
                    task_id
                );
            }

        } else if (strcmp(command, "waitall") == 0) {

            printf("Waiting for all tasks...\n");

            taskforge_wait_all(engine);

            printf("All tasks finished.\n");

        } else if (strcmp(command, "stats") == 0) {

            taskforge_print_stats(engine);

        } else if (
            strcmp(command, "shutdown") == 0 ||
            strcmp(command, "exit") == 0
        ) {

            printf("Shutting down TaskForge...\n");

            taskforge_shutdown(engine);

            printf("TaskForge shutdown complete.\n");

            break;

        } else if (strlen(command) == 0) {

            continue;

        } else {

            printf(
                "Unknown command: %s\n",
                command
            );

            printf(
                "Type 'help' to see available commands.\n"
            );
        }
    }
}
