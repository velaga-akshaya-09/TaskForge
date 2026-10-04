#ifndef STATS_H
#define STATS_H

#include <pthread.h>

typedef struct {

    unsigned long submitted;

    unsigned long completed;

    unsigned long failed;

    unsigned long cancelled;

    unsigned long peak_queue_size;

    unsigned long total_execution_time_us;

    unsigned long max_execution_time_us;

} TaskForgeStats;


/* Initialize statistics */
void stats_init(TaskForgeStats *stats);


/* Record a submitted task */
void stats_task_submitted(
    TaskForgeStats *stats,
    unsigned long queue_size
);


/* Record a completed task */
void stats_task_completed(
    TaskForgeStats *stats,
    unsigned long execution_time_us
);


/* Record a failed task */
void stats_task_failed(
    TaskForgeStats *stats,
    unsigned long execution_time_us
);


/* Record a cancelled task */
void stats_task_cancelled(
    TaskForgeStats *stats
);


/* Calculate average execution time */
double stats_average_execution_time(
    const TaskForgeStats *stats
);

#endif
