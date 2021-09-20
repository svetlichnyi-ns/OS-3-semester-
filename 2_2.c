#define _GNU_SOURCE

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>  
#include <errno.h> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define RESULT_OK  0
#define RESULT_BAD_ARGS 1
#define RESULT_OPEN_FAILURE 2
#define RESULT_WRITING_FAILURE 3
#define RESULT_CLOSE_FAILURE 4

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <text-to-write>\n", argv[0]);
        return RESULT_BAD_ARGS;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        fprintf(stderr, "Error: cannot open the file '%s'\n", argv[1]);
        printf("Name of error: ");
        switch (errno) {
            case 13:  printf("EACCES\n");       break;
            case 122: printf("EDQUOT\n");       break;
            case 17:  printf("EEXIST\n");       break;
            case 14:  printf("EFAULT\n");       break;
            case 27:  printf("EFBIG\n");        break;
            case 4:   printf("EINTR\n");        break;
            case 22:  printf("EINVAL\n");       break;
            case 21:  printf("EISDIR\n");       break;
            case 40:  printf("ELOOP\n");        break;
            case 24:  printf("EMFILE\n");       break;
            case 36:  printf("ENAMETOOLONG\n"); break;
            case 23:  printf("ENFILE\n");       break;
            case 19:  printf("ENODEV\n");       break;
            case 2:   printf("ENOENT\n");       break;
            case 12:  printf("ENOMEM\n");       break;
            case 28:  printf("ENOSPC\n");       break;
            case 20:  printf("ENOTDIR\n");      break;
            case 6:   printf("ENXIO\n");        break;
            case 95:  printf("EOPNOTSUPP\n");   break;
            case 75:  printf("EOVERFLOW\n");    break;
            case 1:   printf("EPERM\n");        break;
            case 30:  printf("EROFS\n");        break;
            case 26:  printf("ETXTBSY\n");      break;
            case 11:  printf("EWOULDBLOCK\n");  break;
            default:  printf("unknown\n");      break;
        }
        return RESULT_OPEN_FAILURE;
    }
    if (dprintf(fd, "%s", argv[2]) < 0) {
        perror("Failure while writing");
        close(fd);
        return RESULT_WRITING_FAILURE;
    }
    if (close(fd) < 0) {
        fprintf(stderr, "Error: cannot close the second file '%s'\n", argv[1]);
        printf("Name of error: ");
        switch (errno) {
            case 9:  printf("EBADF\n");       break;
            case 4:  printf("EINTR\n");       break;
            case 5:  printf("EIO\n");         break;
            default: printf("unknown\n");     break;
        }
        return RESULT_CLOSE_FAILURE;
    }
    printf("Data has been successfully written to the file!\n");
    return RESULT_OK;
}
