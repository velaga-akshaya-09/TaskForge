#include "stats.h"

void stats_init(TaskForgeStats *stats)
{
    stats->submitted = 0;
    stats->completed = 0;
    stats->failed = 0;
    stats->cancelled = 0;

    stats->peak_queue_size = 0;

    stats->total_execution_time_us = 0;
    stats->max_execution_time_us = 0;
}

void stats_task_submitted(
    TaskForgeStats *stats,
    unsigned long queue_size
)
{
    stats->submitted++;

    if (queue_size > stats->peak_queue_size) {
        stats->peak_queue_size = queue_size;
    }
}

void stats_task_completed(
    TaskForgeStats *stats,
    unsigned long execution_time_us
)
{
    stats->completed++;

    stats->total_execution_time_us += execution_time_us;

    if (execution_time_us > stats->max_execution_time_us) {
        stats->max_execution_time_us = execution_time_us;
    }
}

void stats_task_failed(
    TaskForgeStats *stats,
    unsigned long execution_time_us
)
{
    stats->failed++;

    stats->total_execution_time_us += execution_time_us;

    if (execution_time_us > stats->max_execution_time_us) {
        stats->max_execution_time_us = execution_time_us;
    }
}

void stats_task_cancelled(TaskForgeStats *stats)
{
    stats->cancelled++;
}

double stats_average_execution_time(
    const TaskForgeStats *stats
)
{
    unsigned long finished_tasks;

    finished_tasks = stats->completed + stats->failed;

    if (finished_tasks == 0) {
        return 0.0;
    }

    return (double)stats->total_execution_time_us /
           (double)finished_tasks;
}
