#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int status;

    printf("========================================\n");
    printf("    TASKFORGE PROCESS LIFECYCLE DEMO\n");
    printf("========================================\n\n");

    printf("[PARENT] Initial state: RUNNING\n");
    printf("[PARENT] PID: %d\n\n", getpid());

    /*
     * Process creation.
     */
    printf("[PARENT] Creating child using fork()...\n");

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    /*
     * Child process.
     */
    if (pid == 0) {

        printf("\n[CHILD] State: CREATED -> RUNNING\n");
        printf("[CHILD] PID: %d\n", getpid());
        printf("[CHILD] Parent PID: %d\n", getppid());

        /*
         * Simulate work performed by the child.
         */
        printf("[CHILD] Performing work...\n");

        sleep(2);

        printf("[CHILD] Work completed.\n");
        printf("[CHILD] State: RUNNING -> TERMINATED\n");

        exit(0);
    }

    /*
     * Parent process waits for child.
     */
    printf("[PARENT] Child PID: %d\n", pid);
    printf("[PARENT] State: RUNNING -> WAITING\n");
    printf("[PARENT] Waiting using waitpid()...\n");

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    /*
     * Child has terminated and parent resumes.
     */
    printf("\n[PARENT] State: WAITING -> RUNNING\n");

    if (WIFEXITED(status)) {

        printf("[PARENT] Child terminated normally.\n");
        printf("[PARENT] Child exit status: %d\n",
               WEXITSTATUS(status));
    }

    printf("\n[PARENT] Process lifecycle demonstration completed.\n");

    return 0;
}
