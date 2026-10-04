#include <stdio.h>

typedef struct {
    int id;
    int priority;
    int burst_time;
    int sequence;
} Process;

void print_process(Process p)
{
    printf("P%d\tPriority: %d\tBurst: %d\tSequence: %d\n",
           p.id,
           p.priority,
           p.burst_time,
           p.sequence);
}

void sort_by_priority(Process processes[], int count)
{
    int i;
    int j;

    Process temp;

    /*
     * Higher priority executes first.
     *
     * If priorities are equal, the process
     * with the smaller sequence number
     * executes first (FIFO).
     */

    for (i = 0; i < count - 1; i++) {

        for (j = 0; j < count - i - 1; j++) {

            if (processes[j].priority <
                processes[j + 1].priority) {

                temp = processes[j];

                processes[j] =
                    processes[j + 1];

                processes[j + 1] = temp;
            }

            else if (processes[j].priority ==
                     processes[j + 1].priority &&
                     processes[j].sequence >
                     processes[j + 1].sequence) {

                temp = processes[j];

                processes[j] =
                    processes[j + 1];

                processes[j + 1] = temp;
            }
        }
    }
}

int main(void)
{
    Process processes[] = {
        {1, 1, 4, 1},
        {2, 3, 2, 2},
        {3, 4, 1, 3},
        {4, 3, 3, 4},
        {5, 2, 2, 5}
    };

    int count = sizeof(processes) /
                sizeof(processes[0]);

    int i;

    printf("========================================\n");
    printf("    TASKFORGE PROCESS SCHEDULING DEMO\n");
    printf("========================================\n\n");

    printf("Initial runnable processes:\n");
    printf("---------------------------\n");

    for (i = 0; i < count; i++) {
        print_process(processes[i]);
    }

    sort_by_priority(processes, count);

    printf("\nScheduling policy:\n");
    printf("------------------\n");
    printf("1. Higher priority executes first.\n");
    printf("2. Equal priority uses FIFO ordering.\n");

    printf("\nExecution order:\n");
    printf("-----------------\n");

    for (i = 0; i < count; i++) {

        printf("%d. P%d "
               "(Priority=%d, Burst=%d, Sequence=%d)\n",
               i + 1,
               processes[i].id,
               processes[i].priority,
               processes[i].burst_time,
               processes[i].sequence);
    }

    printf("\nScheduling demonstration completed.\n");

    return 0;
}
