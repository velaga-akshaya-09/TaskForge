#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int *malloc_memory;
    int *calloc_memory;
    int *realloc_memory;

    size_t i;

    printf("========================================\n");
    printf("   TASKFORGE MEMORY ALLOCATION DEMO\n");
    printf("========================================\n\n");

    /*
     * malloc()
     */
    printf("[1] malloc()\n");
    printf("--------------------\n");

    malloc_memory = malloc(5 * sizeof(int));

    if (malloc_memory == NULL) {
        perror("malloc");
        return 1;
    }

    for (i = 0; i < 5; i++) {
        malloc_memory[i] = (int)(i + 1) * 10;
    }

    printf("Allocated 5 integers using malloc().\n");
    printf("Values: ");

    for (i = 0; i < 5; i++) {
        printf("%d ", malloc_memory[i]);
    }

    printf("\n");

    /*
     * calloc()
     */
    printf("\n[2] calloc()\n");
    printf("--------------------\n");

    calloc_memory = calloc(5, sizeof(int));

    if (calloc_memory == NULL) {
        perror("calloc");
        free(malloc_memory);
        return 1;
    }

    printf("Allocated 5 integers using calloc().\n");
    printf("Initial values: ");

    for (i = 0; i < 5; i++) {
        printf("%d ", calloc_memory[i]);
    }

    printf("\n");

    /*
     * realloc()
     */
    printf("\n[3] realloc()\n");
    printf("--------------------\n");

    realloc_memory = malloc(3 * sizeof(int));

    if (realloc_memory == NULL) {
        perror("malloc");
        free(malloc_memory);
        free(calloc_memory);
        return 1;
    }

    for (i = 0; i < 3; i++) {
        realloc_memory[i] = (int)(i + 1) * 100;
    }

    printf("Initially allocated 3 integers.\n");

    int *temporary_memory =
        realloc(realloc_memory, 6 * sizeof(int));

    if (temporary_memory == NULL) {
        perror("realloc");
        free(malloc_memory);
        free(calloc_memory);
        free(realloc_memory);
        return 1;
    }

    realloc_memory = temporary_memory;

    realloc_memory[3] = 400;
    realloc_memory[4] = 500;
    realloc_memory[5] = 600;

    printf("Expanded allocation to 6 integers using realloc().\n");
    printf("Values: ");

    for (i = 0; i < 6; i++) {
        printf("%d ", realloc_memory[i]);
    }

    printf("\n");

    /*
     * Stack demonstration
     */
    printf("\n[4] Stack vs Heap\n");
    printf("--------------------\n");

    int stack_variable = 50;

    printf("Stack variable value : %d\n",
           stack_variable);

    printf("Stack variable address: %p\n",
           (void *)&stack_variable);

    printf("malloc() memory address: %p\n",
           (void *)malloc_memory);

    printf("calloc() memory address: %p\n",
           (void *)calloc_memory);

    printf("realloc() memory address: %p\n",
           (void *)realloc_memory);

    /*
     * free()
     */
    printf("\n[5] free()\n");
    printf("--------------------\n");

    free(malloc_memory);
    malloc_memory = NULL;

    free(calloc_memory);
    calloc_memory = NULL;

    free(realloc_memory);
    realloc_memory = NULL;

    printf("All dynamically allocated memory was released.\n");

    printf("\nDynamic memory allocation demonstration completed.\n");

    return 0;
}
