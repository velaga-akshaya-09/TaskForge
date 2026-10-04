#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    int pipefd[2];
    pid_t pid;

    char message[] =
        "Hello from the parent process through a pipe!";

    char buffer[100];

    int status;

    printf("========================================\n");
    printf("       TASKFORGE PIPE IPC DEMO\n");
    printf("========================================\n\n");

    /*
     * Create an anonymous pipe.
     *
     * pipefd[0] -> read end
     * pipefd[1] -> write end
     */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("[PARENT] Anonymous pipe created.\n");
    printf("[PARENT] Read FD : %d\n", pipefd[0]);
    printf("[PARENT] Write FD: %d\n\n", pipefd[1]);

    /*
     * Create child process.
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

        close(pipefd[1]);

        printf("[CHILD] Waiting for data from parent...\n");

        ssize_t bytes_read = read(
            pipefd[0],
            buffer,
            sizeof(buffer) - 1
        );

        if (bytes_read < 0) {
            perror("read");
            close(pipefd[0]);
            exit(1);
        }

        buffer[bytes_read] = '\0';

        printf("[CHILD] Received message:\n");
        printf("        \"%s\"\n", buffer);

        close(pipefd[0]);

        printf("[CHILD] Read end closed.\n");

        exit(0);
    }

    /*
     * Parent process.
     */
    close(pipefd[0]);

    printf("[PARENT] Sending message to child...\n");

    ssize_t bytes_written = write(
        pipefd[1],
        message,
        strlen(message)
    );

    if (bytes_written < 0) {
        perror("write");
        close(pipefd[1]);
        return 1;
    }

    printf("[PARENT] Bytes written: %zd\n", bytes_written);

    close(pipefd[1]);

    printf("[PARENT] Write end closed.\n");

    /*
     * Wait for child.
     */
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("\n[PARENT] Child exited with status: %d\n",
               WEXITSTATUS(status));
    }

    printf("\nAnonymous pipe IPC demonstration completed.\n");

    return 0;
}
