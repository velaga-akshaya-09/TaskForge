#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *numbers;

    printf("========================================\n");
    printf("     TASKFORGE MEMORY ERROR DEMO\n");
    printf("========================================\n\n");

    numbers = malloc(5 * sizeof(int));

    if (numbers == NULL) {
        perror("malloc");
        return 1;
    }

    numbers[0] = 10;
    numbers[1] = 20;
    numbers[2] = 30;
    numbers[3] = 40;
    numbers[4] = 50;

    printf("[1] Memory allocated successfully.\n");
    printf("[2] Values stored in heap memory.\n");

    free(numbers);

    printf("[3] Memory released using free().\n");

    printf("[4] Attempting to access released memory...\n");

    /*
     * Intentional memory error:
     * numbers points to memory that has already
     * been released.
     */
    printf("Value: %d\n", numbers[0]);

    return 0;
}
