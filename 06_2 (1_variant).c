#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h> 

char d_type_char (unsigned char d_type) {   // this function identificates type of file, using field "d_type" of the struct dirent, filled in by function "readdir"
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

char mode_char (unsigned mode) {   // this function identificates type of file, using field "stx_mode" of the struct statx, filled in by function "statx"
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory-to-investigate>\n", argv[0]);
        return -1;
    }
    if (chdir(argv[1]) == -1) {   // we change current working directory -- now it's a directory, entered in a command line
        perror("chdir");
        return 1;
    }
    DIR* dir_stream = opendir(".");   // get a pointer to the stream of the current working directory
    if (dir_stream == NULL) {
        perror("opendir");
        return -1;
    }
    struct dirent* entry;
    struct statx sbx;
    while ((entry = readdir(dir_stream)) != NULL) {  // if the directory has not ended yet...
        /* Function "readdir" may also return a zero pointer if directory stream descriptor is invalid. 
        But we are sure in its validity, because we've already handled possible errors, returned by "opendir" */ 
        char entry_type = d_type_char(entry->d_type);
        if (entry_type == '?') {   // if field "d_type" didn't provide us with information about filetype, we use statx(2)
            if (statx(dirfd(dir_stream), entry->d_name, AT_SYMLINK_NOFOLLOW, STATX_TYPE, &sbx) == -1) {
                perror("statx");
                return -1;
            }
            entry_type = mode_char(sbx.stx_mode);
        }
        printf("%c %s\n", entry_type, entry->d_name); 
    }
    closedir(dir_stream); // errors are also impossible here
    return 0;
}
