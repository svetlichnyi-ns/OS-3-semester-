#define _GNU_SOURCE

#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main (int argc, char* argv[]) {
    const char* filesystem_path;   // path to an investigated filesystem
    if (argc > 1)
        filesystem_path = argv[1];   // we suppose, that argument "argv[1]" represents the investigated filesystem
    else
        filesystem_path = ".";   // if the user didn't enter any arguments, we investigate a filesystem, represented by current working directory
    struct statvfs stvfs;
    if (statvfs(filesystem_path, &stvfs) < 0) {
        perror("statvfs");
        return -1;
    }
    printf("Filesystem block size:                          %lu\n", stvfs.f_bsize);
    printf("Fragment size:                                  %lu\n", stvfs.f_frsize);
    printf("Size of filesystem in f_frsize units:           %ju\n", (uintmax_t) stvfs.f_blocks);
    printf("Number of free blocks:                          %ju\n", (uintmax_t) stvfs.f_bfree);
    printf("Number of free blocks for unprivileged users:   %ju\n", (uintmax_t) stvfs.f_bavail);
    printf("Number of inodes:                               %ju\n", (uintmax_t) stvfs.f_files);
    printf("Number of free inodes:                          %ju\n", (uintmax_t) stvfs.f_ffree);
    printf("Number of free inodes for unprivileged users:   %ju\n", (uintmax_t) stvfs.f_favail);
    printf("Fulesystem ID:                                  %lu\n", stvfs.f_fsid);
    printf("Mount flags:                                    %lu\n", stvfs.f_flag);
    printf("Maximum filename length:                        %lu\n", stvfs.f_namemax);
    if (argc == 3) {
        const char* file_path = argv[2];  // we suppose, that argument "argv[2]" is a pathname of a file, which must be checked whether it fits a filesystem
        struct statx sbx;
        if (statx(AT_FDCWD, file_path, AT_SYMLINK_NOFOLLOW, STATX_SIZE, &sbx) < 0) {
            perror("statx");
            return -1;
        }
        if ((stvfs.f_favail < 1) || ((uintmax_t) sbx.stx_size > ((uintmax_t) stvfs.f_bavail) * ((uintmax_t) stvfs.f_bsize)))
            printf("The file «%s» doesn't fit to the filesystem\n", file_path);
        else 
            printf("The file «%s» fits to the filesystem\n", file_path);
    }
    return 0;
}
