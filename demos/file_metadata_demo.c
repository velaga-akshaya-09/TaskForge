#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#define FILE_NAME "taskforge_io_demo.txt"

void print_file_type(mode_t mode)
{
    if (S_ISREG(mode)) {
        printf("File type       : Regular file\n");
    } else if (S_ISDIR(mode)) {
        printf("File type       : Directory\n");
    } else if (S_ISLNK(mode)) {
        printf("File type       : Symbolic link\n");
    } else {
        printf("File type       : Other\n");
    }
}

void print_permissions(mode_t mode)
{
    printf("Permissions      : ");

    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");

    printf("\n");
}

int main(void)
{
    struct stat file_info;

    printf("========================================\n");
    printf("   TASKFORGE FILE METADATA / INODE DEMO\n");
    printf("========================================\n\n");

    printf("[1] Reading File Metadata\n");
    printf("----------------------------------------\n");

    if (stat(FILE_NAME, &file_info) == -1) {
        perror("stat");
        printf("\nMake sure %s exists first.\n",
               FILE_NAME);
        return 1;
    }

    printf("File name        : %s\n",
           FILE_NAME);

    printf("Inode number     : %lu\n",
           (unsigned long)file_info.st_ino);

    printf("File size        : %ld bytes\n",
           (long)file_info.st_size);

    printf("Hard links       : %lu\n",
           (unsigned long)file_info.st_nlink);

    printf("Owner UID        : %lu\n",
           (unsigned long)file_info.st_uid);

    printf("Owner GID        : %lu\n",
           (unsigned long)file_info.st_gid);

    print_file_type(file_info.st_mode);

    print_permissions(file_info.st_mode);

    printf("\n[2] File Timestamps\n");
    printf("----------------------------------------\n");

    printf("Last access      : %s",
           ctime(&file_info.st_atime));

    printf("Last modification: %s",
           ctime(&file_info.st_mtime));

    printf("Last status change: %s",
           ctime(&file_info.st_ctime));

    printf("\n[3] Inode Concept\n");
    printf("----------------------------------------\n");

    printf("The inode stores metadata about the file.\n");
    printf("The filename is used to locate the file,\n");
    printf("while the inode contains information such as\n");
    printf("size, permissions, ownership and timestamps.\n");

    printf("\nFile metadata and inode demonstration completed.\n");

    return 0;
}
