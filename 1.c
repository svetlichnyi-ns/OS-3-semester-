#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <math.h>

#define _GNU_SOURCE

const char* file_type(unsigned long int mode) {
    if (S_ISREG (mode))
        return "regular file";
    else if (S_ISDIR (mode))
        return "directory";
    else if (S_ISCHR (mode)) 
        return "character device";
    else if (S_ISBLK (mode)) 
        return "block device";
    else if (S_ISFIFO (mode)) 
        return "FIFO/pipe";
    else if (S_ISLNK (mode)) 
        return "symlink";
    else if (S_ISSOCK (mode)) 
        return "socket";
    else
        return "unknown";
}

void access_mode(unsigned long int mode, char* access) {
    access[0] = (mode & S_IRUSR) ? 'r' : '-';
    access[1] = (mode & S_IWUSR) ? 'w' : '-';
    access[2] = (mode & S_IXUSR) ? 'x' : '-';
    access[3] = (mode & S_IRGRP) ? 'r' : '-';
    access[4] = (mode & S_IWGRP) ? 'w' : '-';
    access[5] = (mode & S_IXGRP) ? 'x' : '-';
    access[6] = (mode & S_IROTH) ? 'r' : '-';
    access[7] = (mode & S_IWOTH) ? 'w' : '-';
    access[8] = (mode & S_IXOTH) ? 'x' : '-';   
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file-to-investigate>\n", argv[0]);
        return -1;
    }
    struct stat statbuf;
    if (fstatat(AT_FDCWD, argv[1], &statbuf, AT_SYMLINK_NOFOLLOW) == -1) {
        perror("fstatat");
        return -1;
    }
    char access_to_file[sizeof("rwxrwxrwx")] = {};
    access_mode(statbuf.st_mode, access_to_file);
    printf("Name of file:               %s\n", argv[1]);
    printf("ID of containing device:    [%jx, %jx]\n", (long) major (statbuf.st_dev), (long) minor (statbuf.st_dev));
    printf("I-node number:              %ju\n", statbuf.st_ino);
    printf("Type of file:               %s\n", file_type(statbuf.st_mode));
    printf("Access mode:                %s\n", access_to_file);
    printf("Link count:                 %ju\n", statbuf.st_nlink);
    printf("UID:                        %ju\n", (long) statbuf.st_uid);
    printf("GID:                        %ju\n", (long) statbuf.st_gid);
    printf("Preferred I/O block size:   %jd bytes\n", statbuf.st_blksize);
    printf("File size:                  %jd bytes\n", statbuf.st_size);
    printf("Blocks allocated:           %jd\n", statbuf.st_blocks);
    printf("Last status change:         %s", ctime(&statbuf.st_ctime));
    printf("Last file access:           %s", ctime(&statbuf.st_atime));
    printf("Last file modification:     %s", ctime(&statbuf.st_mtime));
    return 0;
}
