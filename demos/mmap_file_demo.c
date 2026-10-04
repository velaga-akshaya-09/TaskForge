#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define FILE_NAME "taskforge_mmap.txt"

int main(void)
{
    int fd;
    struct stat file_info;

    char *mapped_memory;

    const char initial_data[] =
        "Hello from TaskForge memory-mapped file I/O!\n";

    size_t file_size;

    printf("========================================\n");
    printf("      TASKFORGE MMAP FILE I/O DEMO\n");
    printf("========================================\n\n");

    printf("[1] Creating File\n");
    printf("----------------------------------------\n");

    fd = open(
        FILE_NAME,
        O_RDWR | O_CREAT | O_TRUNC,
        0644
    );

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("File opened successfully.\n");
    printf("File descriptor: %d\n", fd);

    /*
     * Write initial contents so the file has
     * storage that can be mapped.
     */
    if (write(
            fd,
            initial_data,
            strlen(initial_data)
        ) == -1) {

        perror("write");
        close(fd);
        return 1;
    }

    printf("Initial data written to file.\n");

    /*
     * Obtain the current file size.
     */
    if (fstat(fd, &file_info) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    file_size = (size_t)file_info.st_size;

    printf("File size: %zu bytes\n",
           file_size);

    printf("\n[2] Mapping File Into Memory\n");
    printf("----------------------------------------\n");

    mapped_memory = mmap(
        NULL,
        file_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    if (mapped_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("File mapped successfully.\n");
    printf("Mapped address: %p\n",
           (void *)mapped_memory);

    printf("\n[3] Reading Through Mapped Memory\n");
    printf("----------------------------------------\n");

    printf("File contents:\n");
    printf("%.*s",
           (int)file_size,
           mapped_memory);

    printf("\n[4] Modifying File Through Memory\n");
    printf("----------------------------------------\n");

    const char replacement[] =
        "Modified through mmap()!\n";

    size_t replacement_size =
        strlen(replacement);

    if (replacement_size <= file_size) {

        memcpy(
            mapped_memory,
            replacement,
            replacement_size
        );

        /*
         * Fill any remaining bytes with spaces
         * so the original data does not remain.
         */
        if (replacement_size < file_size) {

            memset(
                mapped_memory + replacement_size,
                ' ',
                file_size - replacement_size
            );
        }

        printf("File contents modified through mapped memory.\n");

    } else {

        printf("Replacement text is larger than the file.\n");
        printf("No modification performed.\n");
    }

    printf("\n[5] Synchronizing Mapping\n");
    printf("----------------------------------------\n");

    if (msync(
            mapped_memory,
            file_size,
            MS_SYNC
        ) == -1) {

        perror("msync");
        munmap(mapped_memory, file_size);
        close(fd);
        return 1;
    }

    printf("Mapped changes synchronized with the file.\n");

    printf("\n[6] Unmapping and Closing\n");
    printf("----------------------------------------\n");

    if (munmap(
            mapped_memory,
            file_size
        ) == -1) {

        perror("munmap");
        close(fd);
        return 1;
    }

    printf("Memory mapping released.\n");

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    printf("File descriptor closed.\n");

    printf("\n[7] Verifying File Contents\n");
    printf("----------------------------------------\n");

    fd = open(
        FILE_NAME,
        O_RDONLY
    );

    if (fd == -1) {
        perror("open");
        return 1;
    }

    char verify_buffer[200];

    ssize_t bytes_read = read(
        fd,
        verify_buffer,
        sizeof(verify_buffer) - 1
    );

    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    verify_buffer[bytes_read] = '\0';

    printf("File after mmap() modification:\n");
    printf("\"%s\"\n", verify_buffer);

    close(fd);

    printf("\nMemory-mapped file I/O demonstration completed.\n");

    return 0;
}
