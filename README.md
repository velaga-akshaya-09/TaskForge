# TaskForge — A Thread-Pool Task Engine and Linux Systems Programming Demonstrator

TaskForge is a lightweight C-based task execution engine built using POSIX threads. It provides a reusable worker-thread pool, priority-based task scheduling, task cancellation, synchronization, task monitoring, statistics, and an interactive command-line interface.

In addition to the core thread-pool engine, TaskForge contains a collection of Linux systems-programming demonstrations covering processes, IPC, memory management, file systems, file I/O, concurrency, and synchronization.

The project is designed to provide practical demonstrations of operating-system concepts using standard C, Linux system calls, POSIX APIs, and pthreads.

---

## Project Objectives

TaskForge demonstrates:

* Operating-system services and system calls
* User-space and kernel-service interaction
* Process creation and process lifecycle
* Process scheduling concepts
* Inter-process communication
* Signals and asynchronous notifications
* Process groups and sessions
* Virtual memory concepts
* Dynamic memory allocation
* Page faults and demand paging
* Copy-on-write
* Memory debugging
* Linux file descriptors and file I/O
* Inodes and file metadata
* Virtual File System concepts
* Buffered and system-call-based I/O
* Memory-mapped files
* POSIX threads
* Race conditions
* Mutexes
* Condition variables
* Counting semaphores
* Deadlocks
* Read-write locks and advanced synchronization

---

# 1. TaskForge Core Engine

The main TaskForge engine implements a reusable worker-thread pool.

## Core architecture

```text
                         USER
                          |
                          v
                 +----------------+
                 | Interactive CLI|
                 |    cli.c       |
                 +-------+--------+
                         |
                         v
                 +----------------+
                 |   TaskForge    |
                 |  taskforge.c   |
                 +-------+--------+
                         |
             +-----------+-----------+
             |                       |
             v                       v
      +-------------+        +--------------+
      | Task Queue  |        | Task Registry|
      |  queue.c    |        |              |
      +------+------+        +--------------+
             |
             v
      +-------------+
      | Scheduler   |
      |scheduler.c  |
      +------+------+
             |
             v
      +--------------------+
      | Worker Threads     |
      |    worker.c        |
      +---------+----------+
                |
                v
             TASKS
```

---

# 2. Core Features

## Worker Thread Pool

TaskForge creates a fixed number of reusable worker threads.

Workers:

1. Wait for tasks.
2. Receive a task from the shared queue.
3. Execute the task.
4. Update the task state.
5. Update statistics.
6. Notify waiting threads.
7. Execute cleanup callbacks when required.

---

## Priority Scheduling

Tasks have four priority levels:

```text
LOW       = 1
NORMAL    = 2
HIGH      = 3
CRITICAL  = 4
```

Higher-priority tasks execute first.

Tasks having the same priority are scheduled using FIFO ordering.

---

## Task States

Every task can move through the following states:

```text
QUEUED
   |
   v
RUNNING
   |
   +------------+
   |            |
   v            v
COMPLETED    FAILED
```

Queued tasks can also be cancelled:

```text
QUEUED
   |
   v
CANCELLED
```

---

## Task Cancellation

Queued tasks can be cancelled before a worker executes them.

The engine maintains cancellation statistics separately from completed and failed tasks.

---

## Waiting

TaskForge supports:

```text
wait <id>
```

for waiting on an individual task.

It also supports:

```text
waitall
```

for waiting until all submitted tasks finish.

---

## Statistics

The engine tracks:

* Submitted tasks
* Completed tasks
* Failed tasks
* Cancelled tasks
* Peak queue size
* Total execution time
* Maximum execution time
* Average execution time

---

# 3. Interactive CLI

The TaskForge command-line interface provides:

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

Example:

```text
taskforge> submit
taskforge> status 1
taskforge> stats
taskforge> waitall
taskforge> shutdown
```

---

# 4. Operating System and Systems Programming Demonstrations

TaskForge contains practical demonstrations corresponding to operating-system concepts.

All demonstrations are located inside:

```text
demos/
```

---

# CO-1 — The OS as a Service Layer

## Topics Covered

* Operating system as a service abstraction
* User space and kernel services
* System calls
* Linux system information
* Shell role in command execution
* `fork()`
* `exec()`
* `waitpid()`
* Systems programming fundamentals

## Demonstrations

### `os_service_demo.c`

Demonstrates the relationship:

```text
User Application
       |
       v
System Call Interface
       |
       v
Linux Kernel Services
```

It uses Linux system calls such as:

```c
syscall()
write()
getpid()
getppid()
```

It also retrieves Linux system information using `uname()`.

Run:

```bash
./os_service_demo
```

---

### `command_execution_demo.c`

Demonstrates a simplified command execution journey:

```text
User Command
     |
     v
Shell/User Program
     |
     v
fork()
     |
     v
Child Process
     |
     v
exec()
     |
     v
Command Program
     |
     v
waitpid()
     |
     v
Parent Continues
```

Run:

```bash
./command_execution_demo
```

---

# CO-2 — Processes and Process Control

## Topics Covered

* Process abstraction
* Process creation
* Process execution
* Process lifecycle
* Process synchronization
* Process termination
* Process scheduling
* Process management pitfalls
* Zombie processes

## Demonstrations

### `process_lifecycle_demo.c`

Demonstrates:

```text
CREATED
   |
   v
RUNNING
   |
   v
WAITING
   |
   v
TERMINATED
```

Uses:

```c
fork()
waitpid()
exit()
```

Run:

```bash
./process_lifecycle_demo
```

---

### `process_scheduling_demo.c`

Demonstrates priority-based scheduling with FIFO ordering for equal priorities.

Scheduling rule:

```text
Higher Priority
      ↓
First

Same Priority
      ↓
FIFO
```

Run:

```bash
./process_scheduling_demo
```

---

### `process_pitfall_demo.c`

Demonstrates the zombie-process problem and explains why the parent should collect terminated children using `wait()` or `waitpid()`.

Run:

```bash
./process_pitfall_demo
```

---

# CO-3 — Inter-Process Communication

## Topics Covered

* IPC
* Anonymous pipes
* Named pipes / FIFOs
* POSIX signals
* Signal handlers
* Asynchronous notifications
* Process groups
* Sessions

## Demonstrations

### `pipe_demo.c`

Demonstrates anonymous pipe communication:

```text
Parent
  |
  | write()
  v
Anonymous Pipe
  |
  | read()
  v
Child
```

Run:

```bash
./pipe_demo
```

---

### `fifo_demo.c`

Demonstrates named-pipe communication using:

```c
mkfifo()
open()
read()
write()
```

Run:

```bash
./fifo_demo
```

---

### `signal_demo.c`

Demonstrates POSIX signals using:

```c
sigaction()
kill()
pause()
SIGUSR1
```

Communication:

```text
Parent
  |
  | SIGUSR1
  v
Child
  |
  v
Signal Handler
```

Run:

```bash
./signal_demo
```

---

### `process_group_demo.c`

Demonstrates:

```c
setpgid()
setsid()
getpgrp()
getsid()
```

It shows the relationship between:

* Processes
* Process groups
* Sessions

Run:

```bash
./process_group_demo
```

---

# CO-4 — Memory Management

## Topics Covered

* Virtual memory
* Linux process address space
* Dynamic memory allocation
* `malloc()`
* `calloc()`
* `realloc()`
* `free()`
* Memory mapping
* Page faults
* Demand paging behavior
* Copy-on-write
* Memory errors
* AddressSanitizer

## Demonstrations

### `memory_allocation_demo.c`

Demonstrates:

```c
malloc()
calloc()
realloc()
free()
```

and compares stack and heap memory.

Run:

```bash
./memory_allocation_demo
```

---

### `address_space_demo.c`

Demonstrates Linux process memory regions such as:

```text
Read-only data
Initialized data
BSS
Heap
Stack
Shared libraries
```

It also reads:

```text
/proc/self/maps
```

Run:

```bash
./address_space_demo
```

---

### `mmap_demo.c`

Demonstrates anonymous memory mapping using:

```c
mmap()
munmap()
```

Run:

```bash
./mmap_demo
```

---

### `page_fault_demo.c`

Maps a number of anonymous pages and touches each page while observing changes in the process's minor page-fault count.

Run:

```bash
./page_fault_demo
```

This demonstrates page-fault behavior and demand allocation at user-space observable level.

---

### `cow_demo.c`

Demonstrates copy-on-write behavior after:

```c
fork()
```

The child modifies its copy of a variable while the parent's value remains unchanged.

Run:

```bash
./cow_demo
```

---

### `memory_error_demo.c`

Contains an intentional use-after-free example.

It is compiled with AddressSanitizer:

```bash
make memory_error_demo
```

Run:

```bash
./memory_error_demo
```

AddressSanitizer reports the invalid heap access.

This demonstration is intentionally expected to terminate with a memory-error report.

---

# CO-5 — File Systems and File I/O in Linux

## Topics Covered

* Unix file abstraction
* File descriptors
* Linux file I/O system calls
* File metadata
* Inodes
* File naming
* VFS concept
* Buffered I/O
* System-call/file-descriptor I/O
* Memory-mapped file I/O
* Filesystem information

## Demonstrations

### `file_io_demo.c`

Demonstrates:

```c
open()
read()
write()
lseek()
close()
```

and shows how Linux represents an opened file using a file descriptor.

Run:

```bash
./file_io_demo
```

---

### `file_metadata_demo.c`

Uses:

```c
stat()
```

to obtain metadata such as:

* Inode number
* File size
* Owner UID
* Owner GID
* Hard-link count
* Permissions
* File type
* Timestamps

Run:

```bash
./file_metadata_demo
```

---

### `buffered_io_demo.c`

Compares:

### Buffered standard I/O

```c
fopen()
fprintf()
fgets()
fclose()
```

with:

### File-descriptor/system-call I/O

```c
open()
write()
read()
close()
```

Run:

```bash
./buffered_io_demo
```

---

### `mmap_file_demo.c`

Demonstrates memory-mapped file access using:

```c
mmap()
msync()
munmap()
```

A file is mapped into the process address space and modified through the mapping.

Run:

```bash
./mmap_file_demo
```

---

### `filesystem_demo.c`

Demonstrates Linux filesystem information using:

```c
statvfs()
statfs()
```

It also explains the role of the Linux Virtual File System:

```text
Application
     |
     v
Linux File APIs
     |
     v
VFS
     |
     v
Filesystem Implementation
```

Run:

```bash
./filesystem_demo
```

The reported filesystem type depends on the environment in which TaskForge is executed.

---

# CO-6 — Concurrency and Synchronization

## Topics Covered

* Concurrency
* Threads and processes
* POSIX threads
* Shared data
* Race conditions
* Mutexes
* Condition variables
* Thread coordination
* Counting semaphores
* Deadlocks
* Concurrency hazards
* Read-write locks
* Advanced synchronization

---

## Core TaskForge Synchronization

The TaskForge engine uses POSIX synchronization primitives including:

```c
pthread_mutex_t
pthread_cond_t
```

The shared task queue and task registry are protected against concurrent access.

Worker threads coordinate using condition variables.

---

### `semaphore_demo.c`

Demonstrates a counting semaphore controlling access to a shared resource.

Uses:

```c
sem_init()
sem_wait()
sem_post()
sem_destroy()
```

Run:

```bash
./semaphore_demo
```

---

### `race_mutex_demo.c`

Demonstrates a race condition using multiple threads modifying shared data.

It then protects the critical section using:

```c
pthread_mutex_lock()
pthread_mutex_unlock()
```

Run:

```bash
./race_mutex_demo
```

---

### `deadlock_demo.c`

Demonstrates a deadlock scenario:

```text
Thread 1:
    Lock A
       ↓
    Request B

Thread 2:
    Lock B
       ↓
    Request A
```

This produces a circular-wait condition.

Timed mutex acquisition is used so the demonstration does not remain blocked indefinitely.

Run:

```bash
./deadlock_demo
```

---

### `advanced_sync_demo.c`

Demonstrates POSIX read-write locks:

```c
pthread_rwlock_rdlock()
pthread_rwlock_wrlock()
pthread_rwlock_unlock()
```

Multiple readers can access shared data concurrently while a writer obtains exclusive access.

Run:

```bash
./advanced_sync_demo
```

---

# 5. Project Directory Structure

```text
TaskForge/
│
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
├── demos/
│   ├── os_service_demo.c
│   ├── command_execution_demo.c
│   ├── process_lifecycle_demo.c
│   ├── process_scheduling_demo.c
│   ├── process_pitfall_demo.c
│   ├── pipe_demo.c
│   ├── fifo_demo.c
│   ├── signal_demo.c
│   ├── process_group_demo.c
│   ├── memory_allocation_demo.c
│   ├── address_space_demo.c
│   ├── mmap_demo.c
│   ├── page_fault_demo.c
│   ├── cow_demo.c
│   ├── memory_error_demo.c
│   ├── file_io_demo.c
│   ├── file_metadata_demo.c
│   ├── buffered_io_demo.c
│   ├── mmap_file_demo.c
│   ├── filesystem_demo.c
│   ├── semaphore_demo.c
│   ├── race_mutex_demo.c
│   ├── deadlock_demo.c
│   └── advanced_sync_demo.c
│
├── Makefile
├── README.md
└── .gitignore
```

---

# 6. Building the Project

Clone the repository:

```bash
git clone https://github.com/velaga-akshaya-09/TaskForge.git
```

Enter the project:

```bash
cd TaskForge
```

Build the main TaskForge application:

```bash
make
```

Build all demonstrations:

```bash
make demos
```

Build the test programs:

```bash
make tests
```

---

# 7. Running TaskForge

```bash
./taskforge
```

Then use:

```text
help
```

to display the available commands.

---

# 8. Running Tests

Run the complete automated test suite:

```bash
make test
```

The test suite includes:

```text
test_taskforge
test_cancel
test_failure
test_stress
```

The tests verify:

* Priority scheduling
* Task cancellation
* Task failure handling
* Large-scale task submission
* Worker-thread execution
* Task statistics

---

# 9. Running CO Demonstrations

Build all demonstrations:

```bash
make demos
```

## CO-1

```bash
./os_service_demo
./command_execution_demo
```

## CO-2

```bash
./process_lifecycle_demo
./process_scheduling_demo
./process_pitfall_demo
```

## CO-3

```bash
./pipe_demo
./fifo_demo
./signal_demo
./process_group_demo
```

## CO-4

```bash
./memory_allocation_demo
./address_space_demo
./mmap_demo
./page_fault_demo
./cow_demo
make memory_error_demo
./memory_error_demo
```

## CO-5

```bash
./file_io_demo
./file_metadata_demo
./buffered_io_demo
./mmap_file_demo
./filesystem_demo
```

## CO-6

```bash
./semaphore_demo
./race_mutex_demo
./deadlock_demo
./advanced_sync_demo
```

---

# 10. Cleaning the Build

Remove generated executables and object files:

```bash
make clean
```

Rebuild everything:

```bash
make
make demos
```

---

# 11. Technologies Used

* C
* Linux
* POSIX API
* POSIX Threads
* pthreads
* Linux system calls
* GCC
* Make
* AddressSanitizer
* Linux `/proc` interface
* IPC mechanisms
* Linux virtual memory interfaces

---

# 12. Learning Outcomes

After completing the project, the implementation and demonstrations provide practical experience with:

1. Linux system programming
2. Processes and process control
3. Inter-process communication
4. Virtual memory concepts
5. Dynamic memory management
6. Linux file I/O
7. File metadata and filesystem interfaces
8. POSIX threads
9. Synchronization primitives
10. Race conditions and deadlocks
11. Memory debugging
12. Concurrent task execution

---

# 13. Important Technical Notes

The demonstrations operate primarily from user space using standard Linux and POSIX interfaces.

Some operating-system internals cannot be directly inspected from an ordinary user-space application. Therefore:

* The page-fault demonstration observes process-level minor page-fault counts rather than directly displaying kernel page tables.
* The copy-on-write demonstration demonstrates the process-visible behavior of COW; identical virtual addresses alone do not prove identical physical pages.
* The filesystem demonstration reports filesystem information exposed by Linux. The underlying storage implementation depends on the execution environment.
* System-call/file-descriptor I/O is described as "unbuffered" relative to C stdio; Linux may still cache file data inside the kernel.
* The memory-error demonstration intentionally triggers an invalid memory access and is expected to produce an AddressSanitizer report.

---

# 14. Conclusion

TaskForge combines a practical thread-pool task engine with Linux systems-programming demonstrations.

The core engine demonstrates real concurrent task execution using POSIX threads, mutexes, condition variables, priority scheduling, cancellation, task waiting, failure handling, statistics, and cleanup.

The additional demonstrations extend the project into operating-system concepts involving processes, IPC, memory management, file systems, file I/O, concurrency, and synchronization.

The project therefore serves as both:

```text
A practical C thread-pool implementation
                 +
A Linux operating-system concepts demonstrator
```
