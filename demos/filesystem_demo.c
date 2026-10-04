#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/statfs.h>

#define PATH_NAME "."

int main(void)
{
    struct statvfs vfs_info;
    struct statfs fs_info;

    unsigned long long total_space;
    unsigned long long free_space;
    unsigned long long available_space;

    printf("========================================\n");
    printf("    TASKFORGE FILESYSTEM / VFS DEMO\n");
    printf("========================================\n\n");

    printf("[1] Filesystem Information\n");
    printf("----------------------------------------\n");

    if (statvfs(PATH_NAME, &vfs_info) == -1) {
        perror("statvfs");
        return 1;
    }

    if (statfs(PATH_NAME, &fs_info) == -1) {
        perror("statfs");
        return 1;
    }

    printf("Path inspected       : %s\n",
           PATH_NAME);

    printf("Filesystem block size : %lu bytes\n",
           (unsigned long)vfs_info.f_bsize);

    printf("Fragment size         : %lu bytes\n",
           (unsigned long)vfs_info.f_frsize);

    printf("Total blocks          : %lu\n",
           (unsigned long)vfs_info.f_blocks);

    printf("Free blocks           : %lu\n",
           (unsigned long)vfs_info.f_bfree);

    printf("Available blocks      : %lu\n",
           (unsigned long)vfs_info.f_bavail);

    printf("Total file nodes      : %lu\n",
           (unsigned long)vfs_info.f_files);

    printf("Free file nodes       : %lu\n",
           (unsigned long)vfs_info.f_ffree);

    printf("\n[2] Filesystem Space\n");
    printf("----------------------------------------\n");

    total_space =
        (unsigned long long)vfs_info.f_blocks *
        (unsigned long long)vfs_info.f_frsize;

    free_space =
        (unsigned long long)vfs_info.f_bfree *
        (unsigned long long)vfs_info.f_frsize;

    available_space =
        (unsigned long long)vfs_info.f_bavail *
        (unsigned long long)vfs_info.f_frsize;

    printf("Total space           : %llu bytes\n",
           total_space);

    printf("Free space            : %llu bytes\n",
           free_space);

    printf("Available to process  : %llu bytes\n",
           available_space);

    printf("\n[3] Filesystem Type\n");
    printf("----------------------------------------\n");

    printf("Filesystem magic      : 0x%lx\n",
           (unsigned long)fs_info.f_type);

    printf("\nThe filesystem type value is supplied by\n");
    printf("the Linux filesystem interface for the\n");
    printf("filesystem containing the inspected path.\n");

    printf("\n[4] VFS Concept\n");
    printf("----------------------------------------\n");

    printf("Linux provides a Virtual File System (VFS)\n");
    printf("interface that gives applications a common\n");
    printf("way to access different filesystem types.\n");

    printf("\nApplication\n");
    printf("    |\n");
    printf("    v\n");
    printf("Linux file APIs\n");
    printf("    |\n");
    printf("    v\n");
    printf("Virtual File System (VFS)\n");
    printf("    |\n");
    printf("    v\n");
    printf("Filesystem implementation\n");

    printf("\nFilesystem and VFS demonstration completed.\n");

    return 0;
}
