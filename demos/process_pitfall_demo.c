#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int status;

    printf("========================================\n");
    printf("    TASKFORGE PROCESS PITFALL DEMO\n");
    printf("========================================\n\n");

    printf("This demonstration explains a zombie process.\n\n");

    /*
     * Create a child process.
     */
    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    /*
     * Child process.
     */
    if (pid == 0) {

        printf("[CHILD] PID: %d\n", getpid());
        printf("[CHILD] Performing a short task...\n");

        sleep(1);

        printf("[CHILD] Task completed.\n");
        printf("[CHILD] Exiting now.\n");

        exit(0);
    }

    /*
     * Parent process deliberately delays
     * collecting the child's exit status.
     */
    printf("[PARENT] Child PID: %d\n", pid);
    printf("[PARENT] Child has started.\n");

    printf("[PARENT] Delaying waitpid() briefly.\n");
    printf("[PARENT] During this period, a terminated child\n");
    printf("         can temporarily become a zombie.\n\n");

    sleep(2);

    /*
     * Parent finally collects the child.
     */
    printf("[PARENT] Calling waitpid()...\n");

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("[PARENT] Child collected successfully.\n");
        printf("[PARENT] Child exit status: %d\n",
               WEXITSTATUS(status));
    }

    printf("\nProcess-management pitfall demonstration completed.\n");

    printf("\nKey lesson:\n");
    printf("A parent should collect terminated children using\n");
    printf("wait() or waitpid() to avoid leaving zombie processes.\n");

    return 0;
}
