#include "task.h"

const char *task_state_to_string(TaskState state)
{
    switch (state) {
        case TASK_QUEUED:
            return "QUEUED";

        case TASK_RUNNING:
            return "RUNNING";

        case TASK_COMPLETED:
            return "COMPLETED";

        case TASK_FAILED:
            return "FAILED";

        case TASK_CANCELLED:
            return "CANCELLED";

        default:
            return "UNKNOWN";
    }
}

const char *task_priority_to_string(TaskPriority priority)
{
    switch (priority) {
        case PRIORITY_LOW:
            return "LOW";

        case PRIORITY_NORMAL:
            return "NORMAL";

        case PRIORITY_HIGH:
            return "HIGH";

        case PRIORITY_CRITICAL:
            return "CRITICAL";

        default:
            return "UNKNOWN";
    }
}
