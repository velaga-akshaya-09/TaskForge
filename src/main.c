#include "taskforge.h"
#include "cli.h"

#include <stdio.h>

int main(void)
{
    TaskForge engine;

    printf("\n");
    printf("========================================\n");
    printf("          TASKFORGE ENGINE\n");
    printf("========================================\n");
    printf("Initializing worker pool...\n");

    if (taskforge_init(&engine, 3) != 0) {

        printf("Failed to initialize TaskForge.\n");

        return 1;
    }

    printf("Worker threads: 3\n");
    printf("Engine status : READY\n");

    cli_run(&engine);

    printf("========================================\n");

    return 0;
}
