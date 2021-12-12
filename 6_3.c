#define _GNU_SOURCE

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

struct linux_dirent {
    unsigned long  d_ino;     // inode number
    unsigned long  d_off;     // offset to next linux_dirent structure
    unsigned short d_reclen;  // length of this linux_dirent structure
    char           d_name[];  // a null-terminated filename
};

char d_type_char (unsigned char d_type) {   // this function identificates type of file, using variable "d_type"
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

char mode_char (unsigned mode) {   // this function identificates type of file, using field "st_mode" of the struct stat, filled in by function "fstatat"
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

int main(int argc, char *argv[]) {
    int nread; // the number of bytes read
    char buf[BUFSIZ];  // an intermediate buffer for reading from the directory
    struct linux_dirent *entry;
    int bpos;  // current position in buffer
    char d_type;
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory-to-investigate>\n", argv[0]);
        return -1;
    }
    
    DIR* dir_stream = opendir(argv[1]);
    if (dir_stream == NULL) {
        perror("opendir");
        return -1;
    }

    while (1) {
        nread = syscall(SYS_getdents, dirfd(dir_stream), buf, BUFSIZ);
        if (nread == -1) {
            perror("getdents");
            closedir(dir_stream);
            return -1;
        }

        if (nread == 0) {   // we reached the end of directory
            break;
        }

        for (bpos = 0; bpos < nread;) {
            entry = (struct linux_dirent *) (buf + bpos);
            d_type = *(buf + bpos + entry->d_reclen - 1);  // char, responsible for file type
            if (d_type_char(d_type) != '?')
                printf("%c ", d_type_char(d_type));
            else {   // if this char can not provide us with information about filetype, we use fstatat(2)
                if (fstatat(dirfd(dir_stream), entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
                    perror("fstatat");
                    return -1;
                }
                printf("%c ", mode_char(sb.st_mode));
            }
            printf("%s\n", (char *) entry->d_name);
            bpos += entry->d_reclen;   // we add to "bpos" the length of the current structure. We're ready to handle the next directory's entry
        }
    }
    if (closedir(dir_stream) == -1) {
        perror("closedir");
        return -1;
    }
    return 0;
}
