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

ssize_t write_all (int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    const uint8_t *buf_addr = buf;
    while (bytes_written < count) {
        ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);
        if (res < 0)
            return res;
        bytes_written = bytes_written + (size_t) res;
    }
    return (ssize_t) bytes_written;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <text-to-write>\n", argv[0]);
        return RESULT_BAD_ARGS;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        perror("open");
        return RESULT_OPEN_FAILURE;
    }
    if (write_all(fd, argv[2], strlen(argv[2])) < 0) {
        perror("write");
        return RESULT_WRITING_FAILURE;
    }
    if (close(fd) < 0) {
        perror("close");
        return RESULT_CLOSE_FAILURE;
    }
    printf("Data has been successfully written to the file!\n");
    return RESULT_OK;
}
