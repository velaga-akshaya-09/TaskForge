#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_initialized = 100;
int global_uninitialized;

const char read_only_data[] = "TaskForge";

void show_memory_address(
    const char *name,
    const void *address
)
{
    printf("%-25s: %p\n", name, address);
}

int main(void)
{
    int stack_variable = 50;
    int *heap_variable;

    FILE *maps_file;
    char line[512];

    printf("========================================\n");
    printf("    TASKFORGE ADDRESS SPACE DEMO\n");
    printf("========================================\n\n");

    printf("Process ID: %d\n\n", (int)getpid());

    printf("[1] Process Memory Regions\n");
    printf("----------------------------------------\n");

    /*
     * Use a normal object inside the code section
     * instead of printing the function address.
     */
    show_memory_address(
        "Read-only data",
        (const void *)read_only_data
    );

    show_memory_address(
        "Initialized data",
        (const void *)&global_initialized
    );

    show_memory_address(
        "BSS data",
        (const void *)&global_uninitialized
    );

    show_memory_address(
        "Stack",
        (const void *)&stack_variable
    );

    heap_variable = malloc(sizeof(int));

    if (heap_variable == NULL) {
        perror("malloc");
        return 1;
    }

    *heap_variable = 123;

    show_memory_address(
        "Heap",
        (const void *)heap_variable
    );

    printf("\n[2] Linux /proc/self/maps\n");
    printf("----------------------------------------\n");

    printf("The following entries show virtual memory\n");
    printf("regions mapped into this process.\n\n");

    maps_file = fopen("/proc/self/maps", "r");

    if (maps_file == NULL) {
        perror("fopen");
        free(heap_variable);
        return 1;
    }

    while (fgets(line, sizeof(line), maps_file) != NULL) {
        printf("%s", line);
    }

    fclose(maps_file);

    free(heap_variable);

    printf("\n[3] Memory Cleanup\n");
    printf("----------------------------------------\n");
    printf("Heap allocation released using free().\n");

    printf("\nLinux process address-space demonstration completed.\n");

    return 0;
}
