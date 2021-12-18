#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

static void handle_events(int fd, int*wd, char* argv[]) {  // this function handles events in one directory, entered by a user in a command line
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event)))); // this buffer must have the same alignment as struct inotify_event
    const struct inotify_event *event;
    ssize_t len;
    char *ptr;
    while (1) {
        len = read(fd, buf, sizeof(buf));
        if ((len == -1) && (errno != EAGAIN)) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (len <= 0)
            break;
        for (ptr = buf; ptr < buf + len; ptr += sizeof (struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;
            if (event->mask & IN_CREATE)
                printf("IN_CREATE: ");
            if (event->mask & IN_MOVED_TO)
                printf("IN_MOVED_TO: ");
            if (wd[1] == event->wd)
                printf("%s/", argv[1]);
            if (event->len)
                printf("%s", event->name);
            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [regular file]\n");
        }
    }
}

int main (int argc, char* argv[]) {
    char buf;
    int fd, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];
    if (argc != 2) {
        printf("Usage: %s PATHNAME\n", argv[0]);
        return -1;
    }
    printf("Enter ENTER for finishing work.\n");
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        return -1;
    }
    wd = calloc(2, sizeof(int));
    if (wd == NULL) {
        perror("calloc");
        return -1;
    }
    wd[1] = inotify_add_watch(fd, argv[1], IN_CREATE | IN_MOVED_TO);
    if (wd[1] == -1) {
        fprintf(stderr, "Impossible to observe '%s'\n", argv[1]);
        perror("inotify_add_watch");
        free(wd);
        return -1;
    }
    nfds = 2;
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = fd;
    fds[1].events = POLLIN;
    printf("Expectation of events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            return -1;
        }
        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }
            if (fds[1].revents & POLLIN) {
                handle_events(fd, wd, argv);
            }
        }
    }
    printf("Expectation of events has been finished.\n");
    if (close(fd) < 0) {
        perror("close");
        return -1;
    }
    free(wd);
    return 0;
}
