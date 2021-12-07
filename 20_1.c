#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define _GNU_SOURCE

int main(void) {
    void *p = mmap(
        NULL, // expected virtual address
        (size_t) sysconf (_SC_PAGE_SIZE), // mapping size
        PROT_READ | PROT_WRITE, // MMU flags
        MAP_ANONYMOUS | MAP_SHARED, // mapping flags
        -1, // file
        0
    );
    if (p == MAP_FAILED) {
        perror("Failed to map a page");
        return 1;
    }
    printf("Allocated page at %p\n", p);

    // define typed pointer
    struct {
        sem_t s;
        unsigned long long i;
        double x, y;
        bool run;
    } *buf = p;
    // write initial content
    sem_init(&buf->s, 1, 1);
    buf->i = 0;
    buf->x = 0;
    buf->y = 0;
    buf->run = true;
    // print content
    printf("1, %llu, %lf %lf\n", buf->i, buf->x, buf->y);

    pid_t child_id = fork();
    switch (child_id) {
        case -1:
            perror("fork");
            return 1;
        case 0:
            while (buf->run) {
                sem_wait(&buf->s);
                buf->i += 1;
                buf->x += 1;
                buf->y -= 1;
                sem_post(&buf->s);
            }
            return 0;
        default:
            for (unsigned i = 0; i < 5; i += 1) {
                sem_wait(&buf->s);
                printf("2, %llu, %lf %lf\n", buf->i, buf->x, buf->y);
                sem_post(&buf->s);
                sleep(1);
            }
            buf->run = false;
            wait(NULL);
            printf("3, %llu %lf %lf\n", buf->i, buf->x, buf->y);
    }
    return 0;
}
