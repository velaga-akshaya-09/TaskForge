#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int status;

    printf("========================================\n");
    printf("   TASKFORGE COMMAND EXECUTION DEMO\n");
    printf("========================================\n\n");

    printf("[Shell] User entered a command.\n");

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

        printf("[Child] Child process created.\n");
        printf("[Child] PID: %d\n", getpid());

        printf("[Child] Executing command using exec().\n\n");

        /*
         * Replace the child process image with
         * the Linux 'ls' program.
         */
        execlp("ls", "ls", "-l", "demos", (char *)NULL);

        /*
         * Reached only if exec() fails.
         */
        perror("execlp");

        exit(1);
    }

    /*
     * Parent process.
     */
    printf("[Shell] Waiting for child process...\n");

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    printf("\n[Shell] Child process finished.\n");

    if (WIFEXITED(status)) {
        printf("[Shell] Child exit status: %d\n",
               WEXITSTATUS(status));
    }

    printf("\nCommand execution journey completed.\n");

    return 0;
}
