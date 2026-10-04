#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>

int main(void)
{
    long page_size_long;
    size_t page_size;
    size_t page_count;

    char *memory;

    struct rusage usage_before;
    struct rusage usage_after;

    size_t i;

    printf("========================================\n");
    printf("    TASKFORGE PAGE FAULT DEMO\n");
    printf("========================================\n\n");

    page_size_long = sysconf(_SC_PAGESIZE);

    if (page_size_long == -1) {
        perror("sysconf");
        return 1;
    }

    page_size = (size_t)page_size_long;

    page_count = 1000;

    printf("[1] Memory Configuration\n");
    printf("----------------------------------------\n");
    printf("Page size : %zu bytes\n", page_size);
    printf("Page count: %zu\n", page_count);
    printf("Total size: %zu bytes\n",
           page_size * page_count);

    memory = mmap(
        NULL,
        page_size * page_count,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (memory == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("\n[2] Virtual Memory Allocation\n");
    printf("----------------------------------------\n");
    printf("Memory mapped successfully.\n");
    printf("Address: %p\n", (void *)memory);

    if (getrusage(RUSAGE_SELF, &usage_before) == -1) {
        perror("getrusage");
        munmap(memory, page_size * page_count);
        return 1;
    }

    printf("\n[3] Before Accessing Pages\n");
    printf("----------------------------------------\n");
    printf("Minor page faults: %ld\n",
           usage_before.ru_minflt);

    printf("\n[4] Touching Each Page\n");
    printf("----------------------------------------\n");
    printf("Writing one byte to each mapped page...\n");

    for (i = 0; i < page_count; i++) {
        memory[i * page_size] = (char)(i % 256);
    }

    printf("All %zu pages accessed.\n", page_count);

    if (getrusage(RUSAGE_SELF, &usage_after) == -1) {
        perror("getrusage");
        munmap(memory, page_size * page_count);
        return 1;
    }

    printf("\n[5] After Accessing Pages\n");
    printf("----------------------------------------\n");
    printf("Minor page faults: %ld\n",
           usage_after.ru_minflt);

    printf("Additional minor page faults: %ld\n",
           usage_after.ru_minflt -
           usage_before.ru_minflt);

    printf("\n[6] Memory Cleanup\n");
    printf("----------------------------------------\n");

    if (munmap(
            memory,
            page_size * page_count
        ) == -1) {

        perror("munmap");
        return 1;
    }

    printf("Mapped memory released successfully.\n");

    printf("\nPage fault and demand paging demonstration completed.\n");

    return 0;
}
