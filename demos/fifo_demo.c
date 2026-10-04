#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_PATH "/tmp/taskforge_fifo"

int main(void)
{
    pid_t pid;
    int status;

    const char message[] =
        "Hello from TaskForge through a named FIFO!";

    char buffer[100];

    printf("========================================\n");
    printf("        TASKFORGE FIFO IPC DEMO\n");
    printf("========================================\n\n");

    /*
     * Remove an old FIFO if one exists.
     */
    unlink(FIFO_PATH);

    /*
     * Create the named pipe.
     */
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }

    printf("[MAIN] Named FIFO created:\n");
    printf("       %s\n\n", FIFO_PATH);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        unlink(FIFO_PATH);
        return 1;
    }

    /*
     * Child acts as the reader.
     */
    if (pid == 0) {

        int fd;
        ssize_t bytes_read;

        printf("[READER] Opening FIFO for reading...\n");

        fd = open(FIFO_PATH, O_RDONLY);

        if (fd == -1) {
            perror("open");
            exit(1);
        }

        bytes_read = read(
            fd,
            buffer,
            sizeof(buffer) - 1
        );

        if (bytes_read < 0) {
            perror("read");
            close(fd);
            exit(1);
        }

        buffer[bytes_read] = '\0';

        printf("[READER] Received message:\n");
        printf("         \"%s\"\n", buffer);

        close(fd);

        printf("[READER] FIFO closed.\n");

        exit(0);
    }

    /*
     * Parent acts as the writer.
     */
    sleep(1);

    printf("[WRITER] Opening FIFO for writing...\n");

    int fd = open(FIFO_PATH, O_WRONLY);

    if (fd == -1) {
        perror("open");
        unlink(FIFO_PATH);
        return 1;
    }

    printf("[WRITER] Sending message...\n");

    ssize_t bytes_written = write(
        fd,
        message,
        strlen(message)
    );

    if (bytes_written < 0) {
        perror("write");
        close(fd);
        unlink(FIFO_PATH);
        return 1;
    }

    printf("[WRITER] Bytes written: %zd\n",
           bytes_written);

    close(fd);

    printf("[WRITER] FIFO closed.\n");

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        unlink(FIFO_PATH);
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("\n[MAIN] Reader exited with status: %d\n",
               WEXITSTATUS(status));
    }

    /*
     * Remove the named pipe.
     */
    unlink(FIFO_PATH);

    printf("\nNamed FIFO IPC demonstration completed.\n");

    return 0;
}
