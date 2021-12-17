#define _GNU_SOURCE

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

char d_type_char (unsigned char d_type) {
    switch (d_type) {
        case DT_BLK:     return 'b'; break;
        case DT_CHR:     return 'c'; break;
        case DT_DIR:     return 'd'; break;
        case DT_FIFO:    return 'f'; break;
        case DT_LNK:     return 'l'; break;
        case DT_REG:     return 'r'; break;
        case DT_SOCK:    return 's'; break;
        case DT_UNKNOWN: return '?'; break;
    }
    return '?';
}

char mode_char (unsigned mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:     return 'b'; break;
        case S_IFCHR:     return 'c'; break;
        case S_IFDIR:     return 'd'; break;
        case S_IFIFO:     return 'f'; break;
        case S_IFLNK:     return 'l'; break;
        case S_IFREG:     return 'r'; break;
        case S_IFSOCK:    return 's'; break;
        default:          return '?'; break;
    }
}


int analyze_dir(const int fd_of_dir, unsigned char level) {  // this function, calling itself recursively, analyzes all entries in a directory
    DIR* dir_stream = fdopendir(fd_of_dir);   // get a pointer to the directory stream
    if (dir_stream == NULL) {
        perror("fdopendir");
        return -1;
    }
    struct dirent* entry;
    struct statx sbx;
    while ((entry = readdir(dir_stream)) != NULL) {   // until we haven't reached the end of a directory...
        char entry_type = d_type_char(entry->d_type);
        if (entry_type == '?') {
            if (statx(fd_of_dir, entry->d_name, AT_SYMLINK_NOFOLLOW, STATX_TYPE, &sbx) == -1) {
                perror("statx");
                return -1;
            }
            entry_type = mode_char(sbx.stx_mode);
        }

        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {   // we avoid acting with a current working directory and its parent
            printf("%c ", entry_type);
            for (unsigned int j = 1; j <= level; j++)   // print all integers, ending with a current level of nesting
                printf("%d ", j);
            printf("%s\n", entry->d_name);
            if (entry_type == 'd') {   // if the handled entry is a directory...
                int fd_of_interior_dir = openat(fd_of_dir, entry->d_name, O_RDONLY | O_DIRECTORY);   // descriptor of a nesting directory
                if (fd_of_interior_dir == -1) {
                    perror("openat");
                    return -1;
                }
                analyze_dir(fd_of_interior_dir, level + 1);   // a recursive call of function
            }
        }
    }
    closedir(dir_stream);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory-to-investigate>\n", argv[0]);
        return -1;
    }
    unsigned int level = 1;   // level of nesting (starts from 1)
    DIR* dir_stream = opendir(argv[1]);
    if (dir_stream == NULL) {
        perror("opendir");
        return -1;
    }
    analyze_dir(dirfd(dir_stream), level);
    closedir(dir_stream);
    return 0;
}
