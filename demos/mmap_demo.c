#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
    long page_size_long;
    size_t page_size;

    void *mapped_memory;

    printf("========================================\n");
    printf("       TASKFORGE MMAP MEMORY DEMO\n");
    printf("========================================\n\n");

    page_size_long = sysconf(_SC_PAGESIZE);

    if (page_size_long == -1) {
        perror("sysconf");
        return 1;
    }

    page_size = (size_t)page_size_long;

    printf("[1] System Page Information\n");
    printf("----------------------------------------\n");
    printf("Page size: %zu bytes\n", page_size);

    printf("\n[2] Creating Anonymous Memory Mapping\n");
    printf("----------------------------------------\n");

    mapped_memory = mmap(
        NULL,
        page_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (mapped_memory == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("Memory mapping created successfully.\n");
    printf("Mapped address: %p\n", mapped_memory);
    printf("Mapped size   : %zu bytes\n", page_size);

    printf("\n[3] Writing to Mapped Memory\n");
    printf("----------------------------------------\n");

    snprintf(
        (char *)mapped_memory,
        page_size,
        "Hello from TaskForge mmap()!"
    );

    printf("Data written successfully.\n");
    printf("Data: \"%s\"\n", (char *)mapped_memory);

    printf("\n[4] Reading from Mapped Memory\n");
    printf("----------------------------------------\n");

    printf("Mapped memory contains:\n");
    printf("  %s\n", (char *)mapped_memory);

    printf("\n[5] Unmapping Memory\n");
    printf("----------------------------------------\n");

    if (munmap(mapped_memory, page_size) == -1) {
        perror("munmap");
        return 1;
    }

    printf("Memory mapping released using munmap().\n");

    printf("\nMemory mapping demonstration completed.\n");

    return 0;
}
