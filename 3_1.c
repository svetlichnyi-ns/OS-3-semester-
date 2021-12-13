#define _GNU_SOURCE

#include <sys/types.h> // for the type "mode_t"
#include <sys/stat.h>  // for flags S_IXXX
#include <fcntl.h>     // for syscall open() and flags O_XXX
#include <unistd.h>    // for syscalls read() and close()
#include <errno.h>     // for the integer variable "errno"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define RESULT_OK 0
#define RESULT_BAD_ARGS 1
#define RESULT_ERROR_OPENING_FILE 2
#define RESULT_ERROR_WRITING_IN_FILE 3
#define RESULT_ERROR_CLOSING_FILE 4
#define RESULT_ERROR_READING_FILE 5

ssize_t read_all(int fd, void *buf, size_t count) {
    size_t read_bytes = 0;
    uint8_t *buf_addr = buf;
    while (read_bytes < count) {
        ssize_t res = read(fd, buf_addr + read_bytes, count - read_bytes);
        if (res < 0)
            return res;
        read_bytes = read_bytes + (size_t) res;
    }
    return (ssize_t) read_bytes;
}

ssize_t write_all(int fd, const void *buf, size_t count) {
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
        fprintf(stderr, "Error: bad arguments!\n");
        fprintf(stderr, "Usage: %s <name_of_regular_file_where_you_want_to_copy_from> <name_of_regular_file_where_you_want_to_copy_in>\n", argv[0]);
        return RESULT_BAD_ARGS;
    }
    struct stat sb_1;
    if (lstat(argv[1], &sb_1) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);  
    }
    if ((sb_1.st_mode & S_IFMT) != S_IFREG) {
        printf("At least, one of files is not regular!\n");
        return -1;
    }
    int fd_1 = open(argv[1], O_RDONLY, ALLPERMS);
    if (fd_1 < 0) {
        perror("open");
        return RESULT_ERROR_OPENING_FILE;
    }
    FILE* filestream_1 = fdopen(fd_1, "r");
    if (filestream_1 == NULL) {
        fprintf(stderr, "Error: cannot connect filestream_1 with existing file descriptor of the first file '%s'\n", argv[1]);
        return -1;
    }
    fseek(filestream_1, 0, SEEK_END);
    unsigned long int size_of_file = ftell(filestream_1);
    char* buffer = (char*) calloc ((size_t) size_of_file + 1, sizeof(char));
    fseek(filestream_1, 0, SEEK_SET);
    ssize_t read_bytes = read_all(fd_1, buffer, size_of_file);
    if (read_bytes < 0) {
        perror("read");
        free(buffer);
        return RESULT_ERROR_READING_FILE;
    }
    if (close(fd_1) < 0) {
        perror("close");
        free(buffer);
        return RESULT_ERROR_CLOSING_FILE;
    }
    int fd_2 = open(argv[2], O_WRONLY | O_CREAT, ALLPERMS);
    struct stat sb_2;
    if (lstat(argv[2], &sb_2) == -1) {
        perror("lstat");
        free(buffer);
        exit(EXIT_FAILURE);  
    }
    if ((sb_2.st_mode & S_IFMT) != S_IFREG) {
        printf("At least, one of files is not regular!\n");
        free(buffer);
        return -1;
    }
    if (fd_2 < 0) {
        perror("open");
        free(buffer);
        return RESULT_ERROR_OPENING_FILE;
    }
    FILE* filestream_2 = fdopen(fd_2, "w");
    if (filestream_2 == NULL) {
        fprintf(stderr, "Error: cannot connect filestream_2 with existing file descriptor of the second file '%s'\n", argv[1]);
        free(buffer);
        return -1;
    }
    fseek(filestream_2, 0, SEEK_SET);
    if (write_all(fd_2, buffer, strlen(buffer)) < 0) {
        perror("write");
        free(buffer);
        return RESULT_ERROR_WRITING_IN_FILE;
    }
    if (close(fd_2) < 0) {
        perror("close");
        free(buffer);
        return RESULT_ERROR_CLOSING_FILE;
    }
    printf("Data has been successfully copied from the first regular file to the second one!\n");
    free(buffer);
    return RESULT_OK;
}
