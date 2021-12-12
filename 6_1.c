#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

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

int main(void) {
    DIR* dir_stream = opendir(".");
    if (dir_stream == NULL) {
        perror("opendir");
        return 1;
    }
    struct dirent* entry;
    while ((entry = readdir(dir_stream)) != NULL) {
        struct statx sbx;
        char entry_type = d_type_char(entry->d_type);
        if (entry_type == '?') {
            if (statx(dirfd(dir_stream), entry->d_name, AT_SYMLINK_NOFOLLOW, STATX_TYPE, &sbx) == -1) {
                perror("statx");
                return 1;
            }
            entry_type = mode_char(sbx.stx_mode);
        }
        printf("%c %s\n", entry_type, entry->d_name); 
    }
    closedir(dir_stream);
    return 0;
}
