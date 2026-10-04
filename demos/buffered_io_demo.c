#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFERED_FILE "taskforge_buffered.txt"
#define UNBUFFERED_FILE "taskforge_unbuffered.txt"

int main(void)
{
    FILE *file;
    int fd;

    const char buffered_message[] =
        "This data uses C standard buffered I/O.\n";

    const char unbuffered_message[] =
        "This data uses Linux system-call I/O.\n";

    char buffer[100];

    ssize_t bytes_read;

    printf("========================================\n");
    printf("   TASKFORGE BUFFERED / UNBUFFERED DEMO\n");
    printf("========================================\n\n");

    /*
     * ------------------------------------------------
     * 1. BUFFERED I/O
     * ------------------------------------------------
     */

    printf("[1] Buffered I/O\n");
    printf("----------------------------------------\n");

    printf("Using fopen() and fprintf().\n");

    file = fopen(
        BUFFERED_FILE,
        "w"
    );

    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    fprintf(
        file,
        "%s",
        buffered_message
    );

    /*
     * fflush() forces buffered data to be
     * written to the underlying file.
     */
    if (fflush(file) == EOF) {
        perror("fflush");
        fclose(file);
        return 1;
    }

    if (fclose(file) == EOF) {
        perror("fclose");
        return 1;
    }

    printf("Data written using buffered stdio.\n");
    printf("File: %s\n", BUFFERED_FILE);

    /*
     * ------------------------------------------------
     * 2. READ USING BUFFERED I/O
     * ------------------------------------------------
     */

    file = fopen(
        BUFFERED_FILE,
        "r"
    );

    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("fgets");
        fclose(file);
        return 1;
    }

    printf("Read using fgets():\n");
    printf("  %s", buffer);

    fclose(file);

    /*
     * ------------------------------------------------
     * 3. UNBUFFERED / SYSTEM-CALL I/O
     * ------------------------------------------------
     */

    printf("\n[2] Unbuffered / System-Call I/O\n");
    printf("----------------------------------------\n");

    printf("Using open() and write().\n");

    fd = open(
        UNBUFFERED_FILE,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (fd == -1) {
        perror("open");
        return 1;
    }

    ssize_t bytes_written = write(
        fd,
        unbuffered_message,
        strlen(unbuffered_message)
    );

    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    printf("Bytes written using write(): %zd\n",
           bytes_written);

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    printf("File: %s\n", UNBUFFERED_FILE);

    /*
     * ------------------------------------------------
     * 4. READ USING SYSTEM CALL
     * ------------------------------------------------
     */

    printf("\n[3] Reading Using read()\n");
    printf("----------------------------------------\n");

    fd = open(
        UNBUFFERED_FILE,
        O_RDONLY
    );

    if (fd == -1) {
        perror("open");
        return 1;
    }

    bytes_read = read(
        fd,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    buffer[bytes_read] = '\0';

    printf("Bytes read using read(): %zd\n",
           bytes_read);

    printf("File contents:\n");
    printf("  %s", buffer);

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    /*
     * ------------------------------------------------
     * 5. COMPARISON
     * ------------------------------------------------
     */

    printf("\n[4] I/O Layer Comparison\n");
    printf("----------------------------------------\n");

    printf("Buffered I/O:\n");
    printf("  fopen() -> fprintf()/fgets() -> fclose()\n");
    printf("  Uses the C standard I/O library.\n");
    printf("  Provides a user-space buffering layer.\n");

    printf("\nUnbuffered/system-call I/O:\n");
    printf("  open() -> write()/read() -> close()\n");
    printf("  Directly uses Linux file-descriptor system calls.\n");

    printf("\nBuffered and unbuffered I/O demonstration completed.\n");

    return 0;
}
