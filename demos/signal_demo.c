#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t signal_received = 0;

void signal_handler(int signal_number)
{
    const char message[] =
        "[HANDLER] SIGUSR1 received asynchronously!\n";

    if (signal_number == SIGUSR1) {
        signal_received = 1;

        write(
            STDOUT_FILENO,
            message,
            sizeof(message) - 1
        );
    }
}

int main(void)
{
    pid_t pid;
    int status;

    struct sigaction action;

    printf("========================================\n");
    printf("       TASKFORGE SIGNAL IPC DEMO\n");
    printf("========================================\n\n");

    /*
     * Configure the SIGUSR1 signal handler.
     */
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("[PARENT] Signal handler registered for SIGUSR1.\n");

    /*
     * Create child process.
     */
    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    /*
     * Child waits for SIGUSR1.
     */
    if (pid == 0) {

        printf("[CHILD] PID: %d\n", getpid());
        printf("[CHILD] Waiting for SIGUSR1...\n");

        while (!signal_received) {
            pause();
        }

        printf("[CHILD] Signal handler completed.\n");
        printf("[CHILD] Continuing execution after signal.\n");

        exit(0);
    }

    /*
     * Parent sends SIGUSR1.
     */
    sleep(1);

    printf("[PARENT] Sending SIGUSR1 to child PID %d...\n",
           pid);

    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        return 1;
    }

    printf("[PARENT] SIGUSR1 sent successfully.\n");

    /*
     * Wait for child termination.
     */
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("\n[PARENT] Child exited with status: %d\n",
               WEXITSTATUS(status));
    }

    printf("\nSignal and signal-handler demonstration completed.\n");

    return 0;
}
