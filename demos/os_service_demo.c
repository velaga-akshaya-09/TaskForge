#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

int main(void)
{
    pid_t pid;
    pid_t ppid;

    struct utsname system_info;

    const char message[] =
        "\n[System Call] write() executed successfully\n";

    printf("========================================\n");
    printf("      TASKFORGE OS SERVICE DEMO\n");
    printf("========================================\n\n");

    /*
     * User-space program requests information
     * from the operating system.
     */

    pid = syscall(SYS_getpid);
    ppid = syscall(SYS_getppid);

    printf("Process ID       : %d\n", pid);
    printf("Parent Process ID: %d\n", ppid);

    /*
     * uname() obtains information supplied
     * by the Linux kernel.
     */

    if (uname(&system_info) == -1) {
        perror("uname");
        return 1;
    }

    printf("\nLinux System Information\n");
    printf("------------------------\n");
    printf("System           : %s\n", system_info.sysname);
    printf("Node Name        : %s\n", system_info.nodename);
    printf("Kernel Release   : %s\n", system_info.release);
    printf("Kernel Version   : %s\n", system_info.version);
    printf("Machine          : %s\n", system_info.machine);

    /*
     * Direct Linux write system call.
     */

    syscall(
        SYS_write,
        STDOUT_FILENO,
        message,
        sizeof(message) - 1
    );

    printf("\nUser Space -> System Call -> Kernel Service\n");
    printf("The application requested services from Linux\n");
    printf("through the system-call interface.\n");

    return 0;
}
