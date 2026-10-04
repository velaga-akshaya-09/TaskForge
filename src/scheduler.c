#include "scheduler.h"

void scheduler_insert(TaskQueue *queue, Task *task)
{
    Task *current;
    Task *previous;

    task->next = NULL;

    /*
     * Empty queue
     */
    if (queue->front == NULL) {
        queue->front = task;
        queue->rear = task;
        queue->size++;
        return;
    }

    current = queue->front;
    previous = NULL;

    /*
     * Find the correct position.
     *
     * Higher priority comes first.
     *
     * If priorities are equal, the smaller
     * sequence number comes first.
     * This gives us FIFO ordering.
     */
    while (current != NULL) {

        if (task->priority > current->priority) {
            break;
        }

        if (task->priority == current->priority &&
            task->sequence < current->sequence) {
            break;
        }

        previous = current;
        current = current->next;
    }

    /*
     * Insert at the front.
     */
    if (previous == NULL) {

        task->next = queue->front;
        queue->front = task;

    } else {

        /*
         * Insert between previous and current.
         */
        task->next = current;
        previous->next = task;
    }

    /*
     * If inserted at the end,
     * update rear.
     */
    if (task->next == NULL) {
        queue->rear = task;
    }

    queue->size++;
}
