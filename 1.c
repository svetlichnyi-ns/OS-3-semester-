#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <dirent.h>

#ifdef S_BLKSIZE
    #define BL_SIZE S_BLKSIZE
    #else
        #include <sys/param.h>
        #ifdef DEV_BSIZE
            #define BL_SIZE DEV_BSIZE
        #else
            #error "Failed to determine block size"
        #endif
#endif

char* my_time (const struct timespec* time) {
    struct tm* ttime = localtime(&(time->tv_sec));
    char yyyymmdd_hhmmss[sizeof("YYYY-mm-dd HH:MM:SS.mmmmmmmmm")];
    char timezone_str[sizeof("+hhmm")];
    strftime(yyyymmdd_hhmmss, sizeof(yyyymmdd_hhmmss), "%F %T", ttime);
    strftime(timezone_str, sizeof(timezone_str), "%z", ttime);
    char* result;
    if (asprintf(&result, "%s.%09ld %s", yyyymmdd_hhmmss, time->tv_nsec, timezone_str) < 0) {
        perror("asprintf");
        return NULL;
    }
    return result;
}

char* my_birth_time (struct statx_timestamp* time) {
    struct tm* ttime = localtime((time_t*) &(time->tv_sec));
    char yyyymmdd_hhmmss[sizeof("YYYY-mm-dd HH:MM:SS.mmmmmmmmm")];
    char timezone_str[sizeof("+hhmm")];
    strftime(yyyymmdd_hhmmss, sizeof(yyyymmdd_hhmmss), "%F %T", ttime);
    strftime(timezone_str, sizeof(timezone_str), "%z", ttime);
    char* result;
    if (asprintf(&result, "%s.%09u %s", yyyymmdd_hhmmss, time->tv_nsec, timezone_str) < 0) {
        perror("asprintf");
        return NULL;
    }
    return result;
}

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
    struct statx sbx;
    if (statx(AT_FDCWD, argv[1], AT_SYMLINK_NOFOLLOW, STATX_BTIME, &sbx) == -1) {
        perror("statx");
        return -1;
    }
    char access_to_file[sizeof("rwxrwxrwx")] = {};
    access_mode(statbuf.st_mode, access_to_file);
    printf("Name of file:                %s\n", argv[1]);
    printf("ID of containing device:     [%jx, %jx]\n", (long) major (statbuf.st_dev), (long) minor (statbuf.st_dev));
    printf("I-node number:               %ju\n", statbuf.st_ino);
    printf("Type of file:                %s\n", file_type(statbuf.st_mode));
    printf("Access mode:                 %s\n", access_to_file);
    printf("Link count:                  %ju\n", statbuf.st_nlink);
    printf("UID:                         %ju\n", (long) statbuf.st_uid);
    printf("GID:                         %ju\n", (long) statbuf.st_gid);
    printf("Preferred I/O block size:    %jd bytes\n", statbuf.st_blksize);
    printf("Real block size:             %jd bytes\n", (long int) BL_SIZE);
    printf("File size:                   %jd bytes\n", statbuf.st_size);
    printf("Blocks allocated:            %jd\n", statbuf.st_blocks);
    tzset();
    char* time_string;
    time_string = my_time(&(statbuf.st_atim));
    printf("Last access time:            %s\n", time_string);
    free(time_string);
    time_string = my_time(&(statbuf.st_ctim));
    printf("Last status change time:     %s\n", time_string);
    free(time_string);
    time_string = my_time(&(statbuf.st_mtim));
    printf("Last file modification time: %s\n", time_string);
    free(time_string);
    time_string = my_birth_time(&(sbx.stx_btime));
    printf("Birth time:                  %s\n", time_string);
    free(time_string);
    return 0;
}
