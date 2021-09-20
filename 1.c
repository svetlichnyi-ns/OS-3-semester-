#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void everything_about_file(const char* restrict pathname, 
                           struct stat* restrict statbuf, 
                           uintmax_t* group_ID, 
                           uintmax_t* user_ID, 
                           uintmax_t* file_size, 
                           uintmax_t* block_size, 
                           uintmax_t* major_ID, 
                           uintmax_t* minor_ID, 
                           uintmax_t* mode, 
                           uintmax_t* inumber, 
                           uintmax_t* blocks, 
                           uintmax_t* link_count) {
    if (lstat(pathname, statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    *group_ID = statbuf->st_uid;
    *user_ID = statbuf->st_gid;
    *file_size = statbuf->st_size;
    *block_size = statbuf->st_blksize;
    *major_ID = major (statbuf->st_dev);
    *minor_ID = minor (statbuf->st_dev);
    *mode = statbuf->st_mode;
    *inumber = statbuf->st_ino;
    *blocks = statbuf->st_blocks;
    *link_count = statbuf->st_nlink;
    return;
}

char* file_type(const char* restrict pathname, struct stat* restrict statbuf) {
    if (lstat(pathname, statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    if (S_ISREG(statbuf->st_mode))
        return "regular file";
    else if (S_ISDIR(statbuf->st_mode)) 
        return "directory";
    else if (S_ISCHR(statbuf->st_mode)) 
        return "character device";
    else if (S_ISBLK(statbuf->st_mode)) 
        return "block device";
    else if (S_ISFIFO(statbuf->st_mode)) 
        return "FIFO/pipe";
    else if (S_ISLNK(statbuf->st_mode)) 
        return "symlink";
    else if (S_ISSOCK(statbuf->st_mode)) 
        return "socket";
    else
        return "unknown";
}

char* access_mode_for_an_owner(const char* restrict pathname, struct stat* restrict statbuf) {
    if (lstat(pathname, statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    int access_mode_for_an_owner = statbuf->st_mode & 0700;
    if ((access_mode_for_an_owner) == 0700) 
	    return "can read, write and execute";
    else if ((access_mode_for_an_owner) == 0600) 
	    return "can read and write";
    else if ((access_mode_for_an_owner) == 0500) 
	    return "can read and execute";
    else if ((access_mode_for_an_owner) == 0400) 
	    return "can read";
    else if ((access_mode_for_an_owner) == 0300) 
	    return "can write and execute";
    else if ((access_mode_for_an_owner) == 0200) 
	    return "can write";
    else if ((access_mode_for_an_owner) == 0100) 
	    return "can execute";
    else 
        return "unknown";
}

char* access_mode_for_a_group(const char* restrict pathname, struct stat* restrict statbuf) {
    if (lstat(pathname, statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    int access_mode_for_a_group = statbuf->st_mode & 070;
    if ((access_mode_for_a_group) == 070) 
	    return "can read, write and execute";
    else if ((access_mode_for_a_group) == 060) 
	    return "can read and write";
    else if ((access_mode_for_a_group) == 050) 
	    return "can read and execute";
    else if ((access_mode_for_a_group) == 040) 
	    return "can read";
    else if ((access_mode_for_a_group) == 030) 
	    return "can write and execute";
    else if ((access_mode_for_a_group) == 020) 
	    return "can write";
    else if ((access_mode_for_a_group) == 010) 
	    return "can execute";
    else 
        return "unknown";
}

char* access_mode_for_all_others(const char* restrict pathname, struct stat* restrict statbuf) {
    if (lstat(pathname, statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    int access_mode_for_all_others = statbuf->st_mode & 7;
    if ((access_mode_for_all_others) == 7) 
	    return "can read, write and execute";
    else if ((access_mode_for_all_others) == 6) 
	    return "can read and write";
    else if ((access_mode_for_all_others) == 5) 
	    return "can read and execute";
    else if ((access_mode_for_all_others) == 4) 
	    return "can read";
    else if ((access_mode_for_all_others) == 3) 
	    return "can write and execute";
    else if ((access_mode_for_all_others) == 2) 
	    return "can write";
    else if ((access_mode_for_all_others) == 1) 
	    return "can execute";
    else 
        return "unknown";
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct stat sb;
    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    uintmax_t group_ID, user_ID, file_size, block_size, major_ID, minor_ID, mode, inumber, blocks, link_count;
    everything_about_file(argv[1], &sb, &group_ID, &user_ID, &file_size, &block_size, &major_ID, &minor_ID, &mode, &inumber, &blocks, &link_count);
    printf("ID of containing device:    [%jx, %jx]\n", major_ID, minor_ID);
    printf("I-node number:              %ju\n", inumber);
    printf("Mode:                       %jo (octal)\n", mode);
    printf("File type:                  %s\n", file_type(argv[1], &sb));
    printf("Access mode for an owner:   %s\n", access_mode_for_an_owner(argv[1], &sb));
    printf("Access mode for a group:    %s\n", access_mode_for_a_group(argv[1], &sb));
    printf("Access mode for all others: %s\n", access_mode_for_all_others(argv[1], &sb));
    printf("Link count:                 %ju\n", link_count);
    printf("UID:                        %ju\n", user_ID);
    printf("GID:                        %ju\n", group_ID);
    printf("Preferred I/O block size:   %jd bytes\n", block_size);
    printf("File size:                  %jd bytes\n", file_size);
    printf("Blocks allocated:           %jd\n", blocks);
    printf("Last status change:         %s", ctime(&sb.st_ctime));
    printf("Last file access:           %s", ctime(&sb.st_atime));
    printf("Last file modification:     %s", ctime(&sb.st_mtime));
    return 0;
}
