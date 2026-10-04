#include "queue.h"

void queue_init(TaskQueue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

void queue_push(TaskQueue *queue, Task *task)
{
    task->next = NULL;

    if (queue->front == NULL) {
        queue->front = task;
        queue->rear = task;
    } else {
        queue->rear->next = task;
        queue->rear = task;
    }

    queue->size++;
}

Task *queue_pop(TaskQueue *queue)
{
    Task *task;

    if (queue->front == NULL) {
        return NULL;
    }

    task = queue->front;

    queue->front = task->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    task->next = NULL;

    queue->size--;

    return task;
}

int queue_remove(TaskQueue *queue, Task *target)
{
    Task *current;
    Task *previous;

    current = queue->front;
    previous = NULL;

    while (current != NULL) {

        if (current == target) {

            if (previous == NULL) {
                queue->front = current->next;
            } else {
                previous->next = current->next;
            }

            if (current == queue->rear) {
                queue->rear = previous;
            }

            current->next = NULL;

            queue->size--;

            return 1;
        }

        previous = current;
        current = current->next;
    }

    return 0;
}

unsigned int queue_size(const TaskQueue *queue)
{
    return queue->size;
}

int queue_is_empty(const TaskQueue *queue)
{
    return queue->front == NULL;
}

void queue_destroy(TaskQueue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}
