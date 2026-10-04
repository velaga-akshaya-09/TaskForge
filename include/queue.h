#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"

typedef struct {
    Task *front;
    Task *rear;
    unsigned int size;
} TaskQueue;

void queue_init(TaskQueue *queue);

void queue_push(TaskQueue *queue, Task *task);

Task *queue_pop(TaskQueue *queue);

int queue_remove(TaskQueue *queue, Task *target);

unsigned int queue_size(const TaskQueue *queue);

int queue_is_empty(const TaskQueue *queue);

void queue_destroy(TaskQueue *queue);

#endif
