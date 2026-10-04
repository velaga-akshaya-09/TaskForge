# TaskForge — A Thread-Pool Task Engine

## Overview

TaskForge is a lightweight task execution engine written in C using POSIX threads (`pthreads`).

It implements a reusable pool of worker threads that execute submitted tasks from a shared task queue. Instead of creating a new thread for every task, TaskForge creates a fixed number of worker threads and reuses them throughout the lifetime of the engine.

The project demonstrates practical concepts in concurrent programming, operating systems, thread synchronization, scheduling, task management, cancellation, and performance monitoring.

---

## Features

* Fixed-size worker thread pool
* Reusable worker threads
* Thread-safe task submission
* Priority-based task scheduling
* FIFO ordering for tasks with equal priority
* Four task priority levels
* Individual task waiting
* Waiting for all tasks
* Queued-task cancellation
* Task success/failure tracking
* Task execution-time measurement
* Queue peak-size tracking
* Average execution-time statistics
* Maximum execution-time statistics
* Task status inspection
* Cleanup callbacks
* Interactive command-line interface
* Automated functional tests
* Stress testing with 100 tasks
* Graceful worker shutdown

---

## Priority Levels

TaskForge supports four priority levels:

| Priority | Value |
| -------- | ----: |
| LOW      |     1 |
| NORMAL   |     2 |
| HIGH     |     3 |
| CRITICAL |     4 |

Higher-priority tasks are placed ahead of lower-priority tasks in the queue.

If two tasks have the same priority, they are ordered using their submission sequence number. Therefore, tasks with equal priority follow FIFO ordering.

---

## Architecture

TaskForge is divided into several modules.

```text
TaskForge/
├── include/
│   ├── taskforge.h
│   ├── task.h
│   ├── queue.h
│   ├── worker.h
│   ├── scheduler.h
│   ├── stats.h
│   └── cli.h
│
├── src/
│   ├── main.c
│   ├── taskforge.c
│   ├── task.c
│   ├── queue.c
│   ├── worker.c
│   ├── scheduler.c
│   ├── stats.c
│   └── cli.c
│
├── tests/
│   ├── test_taskforge.c
│   ├── test_cancel.c
│   ├── test_failure.c
│   └── test_stress.c
│
├── Makefile
└── README.md
```

---

## Module Description

### `task.h`

Defines the basic task structure.

A task contains:

* Task ID
* Task function
* Task argument
* Cleanup callback
* Priority
* State
* Result
* Cancellation information
* Submission sequence number
* Timing information

Task states include:

```text
QUEUED
RUNNING
COMPLETED
FAILED
CANCELLED
```

---

### `queue.h` / `queue.c`

Implements the linked-list task queue.

The queue supports:

* Initialization
* Insertion
* Removal from the front
* Removing a specific task
* Size checking
* Empty checking
* Destruction

The queue itself provides the basic data structure, while the scheduler determines where a task should be inserted.

---

### `scheduler.h` / `scheduler.c`

Implements priority scheduling.

When a task is submitted, the scheduler inserts it into the queue according to:

1. Higher priority first
2. Earlier submission sequence for equal priority

For example:

```text
Task A → LOW
Task B → HIGH
Task C → NORMAL
Task D → CRITICAL
```

The queue becomes:

```text
Task D → CRITICAL
Task B → HIGH
Task C → NORMAL
Task A → LOW
```

---

### `worker.h` / `worker.c`

Contains the worker-thread implementation.

Each worker repeatedly:

1. Locks the engine mutex.
2. Waits if the queue is empty.
3. Retrieves the next scheduled task.
4. Changes its state to `RUNNING`.
5. Executes the task.
6. Records the execution time.
7. Changes the task state to `COMPLETED` or `FAILED`.
8. Updates statistics.
9. Signals waiting threads.
10. Executes the cleanup callback.

Workers are reused for multiple tasks.

---

### `stats.h` / `stats.c`

Maintains runtime statistics.

Tracked values include:

```text
Submitted
Completed
Failed
Cancelled
Peak queue size
Average execution time
Maximum execution time
```

Execution time is measured in microseconds.

---

### `taskforge.h` / `taskforge.c`

Contains the main TaskForge engine.

The engine manages:

* Worker threads
* Task queue
* Task registry
* Mutex
* Condition variables
* Task IDs
* Submission sequence numbers
* Active task count
* Statistics

Important APIs include:

```c
taskforge_init()
taskforge_submit()
taskforge_wait()
taskforge_cancel()
taskforge_wait_all()
taskforge_print_task()
taskforge_print_stats()
taskforge_shutdown()
```

---

### `cli.h` / `cli.c`

Provides the interactive command-line interface.

Available commands:

```text
help
submit
status <id>
cancel <id>
wait <id>
waitall
stats
shutdown
exit
```

---

## Thread Synchronization

TaskForge uses POSIX synchronization primitives.

### Mutex

A mutex protects shared engine data.

```c
pthread_mutex_t mutex;
```

It prevents multiple threads from modifying the task queue and task states simultaneously.

---

### Condition Variables

TaskForge uses condition variables for communication between workers and waiting threads.

The task-available condition variable:

```c
pthread_cond_t task_available;
```

allows workers to sleep when there are no tasks.

The all-tasks-done condition variable:

```c
pthread_cond_t all_tasks_done;
```

allows the engine to wait until there are no active or queued tasks.

Each task also has a completion condition variable:

```c
pthread_cond_t completed;
```

which allows a caller to wait for a specific task.

---

## Task Lifecycle

A task follows this general lifecycle:

```text
          submit
             |
             v
         QUEUED
             |
             v
         RUNNING
          /     \
         /       \
        v         v
   COMPLETED    FAILED
```

A queued task can also be cancelled:

```text
QUEUED
   |
   | cancel
   v
CANCELLED
```

Running tasks cannot be cancelled by the current API.

---

## Task Submission

When a task is submitted:

1. A `Task` structure is allocated.
2. A unique task ID is assigned.
3. A submission sequence number is assigned.
4. The task is registered.
5. The scheduler inserts it according to priority.
6. Statistics are updated.
7. A worker is notified.

Example:

```text
TaskForge> submit

Task duration in seconds: 2

Priority levels:
1. LOW
2. NORMAL
3. HIGH
4. CRITICAL
Select priority: 3

Task submitted successfully. ID = 1
```

---

## Task Execution

A worker retrieves the highest-priority queued task and executes its function.

Example:

```text
[TASK 1] Started | Worker executing for 2 sec
[TASK 1] Completed
```

The worker records the start and finish timestamps and calculates the execution time.

---

## Task Status

The `status` command displays information about an individual task.

Example:

```text
TaskForge> status 1

Task Information
----------------
ID        : 1
Priority  : HIGH
State     : COMPLETED
Result    : 0
Sequence  : 1
Execution : 2000496 us
```

A result of:

```text
0
```

represents successful execution.

A non-zero result represents a failed task.

---

## Cancellation

TaskForge supports cancellation of queued tasks.

Example:

```text
TaskForge> cancel 10
Task 10 cancelled.
```

A task that is already running cannot be cancelled:

```text
TaskForge> cancel 2
Task 2 cannot be cancelled (already running or finished).
```

This prevents the engine from forcibly terminating a worker while it is executing user code.

---

## Waiting for Tasks

### Wait for one task

```text
wait <id>
```

Example:

```text
TaskForge> wait 1
Task 1 finished.
```

### Wait for all tasks

```text
waitall
```

Example:

```text
TaskForge> waitall
Waiting for all tasks...
All tasks finished.
```

---

## Statistics

The `stats` command displays engine statistics.

Example:

```text
TaskForge> stats

TaskForge Statistics
--------------------
Submitted       : 1
Completed       : 1
Failed          : 0
Cancelled       : 0
Queue Peak      : 1
Average Time    : 2000488.00 us
Maximum Time    : 2000488 us
```

Average execution time is calculated from completed and failed tasks.

Cancelled tasks are not included in execution-time calculations because they were never executed.

---

## Building the Project

The project uses `gcc` and `make`.

Build the main application with:

```bash
make
```

A successful build produces:

```text
taskforge
```

---

## Running TaskForge

Start the interactive engine with:

```bash
./taskforge
```

The engine starts with three worker threads.

Example:

```text
========================================
          TASKFORGE ENGINE
========================================
Initializing worker pool...
Worker threads: 3
Engine status : READY
```

---

## Running Tests

Build and run all automated tests using:

```bash
make test
```

The test suite contains:

### Priority Test

```text
test_taskforge
```

Tests task submission, priorities, execution, and statistics.

### Cancellation Test

```text
test_cancel
```

Tests cancellation of queued tasks.

### Failure Test

```text
test_failure
```

Tests successful and failed task execution.

### Stress Test

```text
test_stress
```

Submits and executes 100 tasks.

Example successful result:

```text
Submitted       : 100
Completed       : 100
Failed          : 0
Cancelled       : 0
Queue Peak      : 97

STRESS TEST PASSED
```

---

## Cleaning the Project

Remove compiled objects, executables, and test binaries using:

```bash
make clean
```

---

## Compiler Configuration

The project is compiled using strict compiler warnings:

```text
-Wall
-Wextra
-Wpedantic
-std=c11
```

POSIX functionality is enabled using:

```text
-D_POSIX_C_SOURCE=200809L
```

The pthread library is linked using:

```text
-pthread
```

---

## Concurrency Model

TaskForge follows a producer-consumer model.

```text
                 +----------------+
                 |    CLI / User  |
                 +-------+--------+
                         |
                         | Submit
                         v
                +-------------------+
                |   Task Scheduler  |
                +---------+---------+
                          |
                          v
                +-------------------+
                |   Shared Queue    |
                +---------+---------+
                          |
              +-----------+-----------+
              |           |           |
              v           v           v
          Worker 1    Worker 2    Worker 3
              |           |           |
              +-----------+-----------+
                          |
                          v
                    Task Execution
```

The workers continuously consume tasks from the shared queue.

---

## Cleanup Callbacks

Each task can optionally provide a cleanup function.

The cleanup callback is used to release task-specific resources after execution or cancellation.

This allows TaskForge to separate:

```text
Task execution
```

from:

```text
Task resource cleanup
```

For example:

```c
taskforge_submit(
    &engine,
    my_task,
    argument,
    my_cleanup,
    PRIORITY_NORMAL
);
```

---

## Design Goals

TaskForge was designed to demonstrate the following systems concepts:

* Multithreading
* Thread pools
* Mutual exclusion
* Condition variables
* Producer-consumer synchronization
* Priority scheduling
* FIFO scheduling
* Task lifecycle management
* Task cancellation
* Failure handling
* Performance measurement
* Resource cleanup
* Graceful shutdown
* Concurrent workload testing

---

## Current Limitations

The current implementation intentionally keeps the design lightweight.

Some limitations include:

* Running tasks cannot be cancelled.
* Completed tasks remain in the task registry until shutdown.
* The shutdown operation is intended to be performed once.
* Priority ordering can only affect tasks that are still waiting in the queue; tasks already running cannot be preempted.
* The interactive CLI is intended for demonstration and testing rather than production use.

---

## Future Improvements

Possible future enhancements include:

* Stronger priority-ordering tests
* Dynamic worker-pool resizing
* Task timeouts
* Retry support for failed tasks
* Task dependencies
* More detailed per-worker statistics
* Queue wait-time statistics
* Improved CLI formatting
* Configuration files
* Logging support
* Graceful handling of more shutdown scenarios
* Automated memory checking
* Additional concurrency tests

---

## Conclusion

TaskForge demonstrates how a thread-pool-based task engine can be implemented from scratch in C using POSIX threads.

The project combines task management, priority scheduling, synchronization, worker-thread execution, cancellation, statistics, cleanup handling, an interactive CLI, and automated stress testing into a single system.

It provides a practical demonstration of operating-system and concurrent-programming concepts while remaining small enough to understand and extend.
