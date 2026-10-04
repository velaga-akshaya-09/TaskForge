#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child_pid;
    pid_t grandchild_pid;

    int status;

    printf("========================================\n");
    printf("   TASKFORGE PROCESS GROUP/SESSION DEMO\n");
    printf("========================================\n\n");

    printf("[PARENT] PID       : %d\n", getpid());
    printf("[PARENT] PPID      : %d\n", getppid());
    printf("[PARENT] PGID      : %d\n", getpgrp());
    printf("[PARENT] SID       : %d\n", getsid(0));

    printf("\n[PARENT] Creating child process...\n");

    child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        return 1;
    }

    if (child_pid == 0) {

        printf("\n[CHILD] PID       : %d\n", getpid());
        printf("[CHILD] PPID      : %d\n", getppid());
        printf("[CHILD] PGID      : %d\n", getpgrp());
        printf("[CHILD] SID       : %d\n", getsid(0));

        printf("\n[CHILD] Creating a new process group...\n");

        if (setpgid(0, 0) == -1) {
            perror("setpgid");
            exit(1);
        }

        printf("[CHILD] After setpgid()\n");
        printf("[CHILD] PID       : %d\n", getpid());
        printf("[CHILD] PGID      : %d\n", getpgrp());
        printf("[CHILD] SID       : %d\n", getsid(0));

        printf("\n[CHILD] Creating grandchild for session demonstration...\n");

        grandchild_pid = fork();

        if (grandchild_pid < 0) {
            perror("fork");
            exit(1);
        }

        if (grandchild_pid == 0) {

            printf("\n[GRANDCHILD] Before setsid()\n");
            printf("[GRANDCHILD] PID       : %d\n", getpid());
            printf("[GRANDCHILD] PPID      : %d\n", getppid());
            printf("[GRANDCHILD] PGID      : %d\n", getpgrp());
            printf("[GRANDCHILD] SID       : %d\n", getsid(0));

            printf("\n[GRANDCHILD] Creating a new session using setsid()...\n");

            if (setsid() == -1) {
                perror("setsid");
                exit(1);
            }

            printf("\n[GRANDCHILD] After setsid()\n");
            printf("[GRANDCHILD] PID       : %d\n", getpid());
            printf("[GRANDCHILD] PPID      : %d\n", getppid());
            printf("[GRANDCHILD] PGID      : %d\n", getpgrp());
            printf("[GRANDCHILD] SID       : %d\n", getsid(0));

            printf("\n[GRANDCHILD] New session created successfully.\n");

            exit(0);
        }

        printf("[CHILD] Grandchild PID: %d\n", grandchild_pid);
        printf("[CHILD] Waiting for grandchild...\n");

        if (waitpid(grandchild_pid, &status, 0) == -1) {
            perror("waitpid");
            exit(1);
        }

        if (WIFEXITED(status)) {
            printf("[CHILD] Grandchild exit status: %d\n",
                   WEXITSTATUS(status));
        }

        exit(0);
    }

    printf("\n[PARENT] Child PID: %d\n", child_pid);
    printf("[PARENT] Waiting for child...\n");

    if (waitpid(child_pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("\n[PARENT] Child exited with status: %d\n",
               WEXITSTATUS(status));
    }

    printf("\nProcess group and session demonstration completed.\n");

    return 0;
}
