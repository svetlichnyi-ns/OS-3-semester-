#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

char dtype_char (unsigned char dtype) {
    switch (dtype) {
        case DT_BLK:     return 'b'; break;
        case DT_CHR:     return 'c'; break;
        case DT_DIR:     return 'd'; break;
        case DT_FIFO:    return 'f'; break;
        case DT_LNK:     return 'l'; break;
        case DT_REG:     return 'r'; break;
        case DT_SOCK:    return 's'; break;
        default:         return '?'; break;
    }
    return '?';
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory-to-investigate>\n", argv[0]);
        return 1;
    }
    if (chdir(argv[1]) == -1) {
        perror("chdir");
        return 1;
    }
    DIR *dir_fd = opendir(".");
    if (dir_fd == NULL) {
        perror("opendir");
        return 1;
    }
    int fd = dirfd(dir_fd); // файловый дескриптор каталога
    struct dirent *entry;
    while ((entry = readdir(dir_fd)) != NULL) {
        struct stat sb;
        struct statx sbx;
        char entry_type = dtype_char(entry->d_type);
        char access_to_file[sizeof("rwxrwxrwx")] = {};
        if (lstat(entry->d_name, &sb) == -1) {
            perror("lstat");
            return 1;
        }
        if (entry_type == '?')
            entry_type = mode_char(sb.st_mode);
        access_mode(sb.st_mode, access_to_file);
        if (statx(fd, entry->d_name, AT_SYMLINK_NOFOLLOW, STATX_ALL, &sbx) == -1) {
            perror("statx");
            return 1;
        }
        printf("%s %c %s %s\n", access_to_file, entry_type, asctime(localtime((const time_t *) &(sbx.stx_btime.tv_sec))), entry->d_name); 
    }
    closedir(dir_fd);
    return 0;
}
