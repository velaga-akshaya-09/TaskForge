#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILE_NAME "taskforge_io_demo.txt"

int main(void)
{
    int fd;
    int read_fd;

    const char message[] =
        "Hello from TaskForge Linux file I/O!\n";

    char buffer[100];

    ssize_t bytes_written;
    ssize_t bytes_read;

    off_t position;

    printf("========================================\n");
    printf("      TASKFORGE FILE I/O DEMO\n");
    printf("========================================\n\n");

    printf("[1] Opening File\n");
    printf("----------------------------------------\n");

    fd = open(
        FILE_NAME,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("File opened successfully.\n");
    printf("File descriptor: %d\n", fd);

    printf("\n[2] Writing Using write()\n");
    printf("----------------------------------------\n");

    bytes_written = write(
        fd,
        message,
        strlen(message)
    );

    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    printf("Bytes written: %zd\n", bytes_written);

    position = lseek(fd, 0, SEEK_CUR);

    if (position == (off_t)-1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    printf("Current file position: %ld\n",
           (long)position);

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    printf("File descriptor closed.\n");

    printf("\n[3] Reopening File for Reading\n");
    printf("----------------------------------------\n");

    read_fd = open(
        FILE_NAME,
        O_RDONLY
    );

    if (read_fd == -1) {
        perror("open");
        return 1;
    }

    printf("Read file descriptor: %d\n", read_fd);

    printf("\n[4] Reading Using read()\n");
    printf("----------------------------------------\n");

    bytes_read = read(
        read_fd,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes_read == -1) {
        perror("read");
        close(read_fd);
        return 1;
    }

    buffer[bytes_read] = '\0';

    printf("Bytes read: %zd\n", bytes_read);
    printf("File contents:\n");
    printf("%s", buffer);

    printf("\n[5] Closing File\n");
    printf("----------------------------------------\n");

    if (close(read_fd) == -1) {
        perror("close");
        return 1;
    }

    printf("Read file descriptor closed.\n");

    printf("\nLinux file descriptor and I/O demonstration completed.\n");
    printf("Created file: %s\n", FILE_NAME);

    return 0;
}
