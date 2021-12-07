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

int main (int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <text-to-write>\n", argv[0]);
        return RESULT_BAD_ARGS;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        perror("open");
        return RESULT_OPEN_FAILURE;
    }
    if (dprintf(fd, "%s", argv[2]) < 0) {
        perror("dprintf");
        close(fd);
        return RESULT_WRITING_FAILURE;
    }
    if (close(fd) < 0) {
        perror("close");
        return RESULT_CLOSE_FAILURE;
    }
    printf("Data has been successfully written to the file!\n");
    return RESULT_OK;
}
