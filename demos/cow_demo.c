#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int shared_value = 100;
    pid_t pid;
    int status;

    printf("========================================\n");
    printf("      TASKFORGE COPY-ON-WRITE DEMO\n");
    printf("========================================\n\n");

    printf("[PARENT] Before fork()\n");
    printf("[PARENT] PID: %d\n", getpid());
    printf("[PARENT] shared_value: %d\n", shared_value);
    printf("[PARENT] Address: %p\n",
           (void *)&shared_value);

    printf("\n[PARENT] Calling fork()...\n");

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {

        printf("\n[CHILD] After fork()\n");
        printf("[CHILD] PID: %d\n", getpid());
        printf("[CHILD] shared_value: %d\n", shared_value);
        printf("[CHILD] Address: %p\n",
               (void *)&shared_value);

        printf("\n[CHILD] Modifying shared_value...\n");

        shared_value = 200;

        printf("[CHILD] New shared_value: %d\n",
               shared_value);

        printf("[CHILD] Address after modification: %p\n",
               (void *)&shared_value);

        printf("\n[CHILD] The child now has its own private copy.\n");

        exit(0);
    }

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    printf("\n[PARENT] Child completed.\n");

    printf("[PARENT] shared_value is still: %d\n",
           shared_value);

    printf("[PARENT] Address: %p\n",
           (void *)&shared_value);

    printf("\nCopy-on-write demonstration completed.\n");

    return 0;
}
