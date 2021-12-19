#define _GNU_SOURCE 
#define _GLIBC_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <pwd.h>
#include <grp.h>

struct sched_attr {
    unsigned int size;          
    unsigned int sched_policy;
    uintmax_t sched_flags;
    int sched_nice;
    unsigned int sched_priority;
    uintmax_t sched_runtime;
    uintmax_t sched_deadline;
    uintmax_t sched_period;
};

void proc_info(void) {
    printf("General information about current process:\n");
    printf("PID:                        %d\n", getpid());
    printf("PPID:                       %d\n", getppid());
    printf("PGID:                       %d\n", getpgid(0));
    printf("SID:                        %d\n", getsid(0));
    printf("EUID:                       %d\n", geteuid());
    printf("UID:                        %d\n", getuid());
    printf("TGID:                       %d\n", gettid()); 
    
    struct passwd* user = getpwuid(getuid());
    if (user == NULL)
        perror("getpwuid");
    else {
        printf("GID:                        %d\n", user->pw_gid);
        printf("Username:                   %c\n", *(user->pw_name));
        printf("User's password:            %c\n", *(user->pw_passwd));
        printf("Information about user:     %c\n", *(user->pw_gecos));
        printf("Home directory:             %c\n", *(user->pw_dir));
        printf("Programm's shell:           %c\n", *(user->pw_shell));
    }

    struct group* group = getgrgid(user->pw_gid);
    if (group == NULL)
        perror("getgrgid");
    else {
        printf("Name of a group:            %c\n", *(group->gr_name));
        printf("Group's password:           %c\n", *(group->gr_passwd));
    }

    printf("\nSome limits:\n");
    struct rlimit limits;
    getrlimit(RLIMIT_AS, &limits);
    printf("Soft limit for virtual memory size:                 %ju bytes\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for virtual memory size:                 %ju bytes\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_CORE, &limits);
    printf("Soft limit for size of file 'core':                 %ju bytes\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for size of file 'core':                 %ju bytes\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_CPU, &limits);
    printf("Soft limit for CPU execution time:                  %ju seconds\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for CPU execution time:                  %ju seconds\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_DATA, &limits);
    printf("Soft limit for process data segment size:           %ju bytes\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for process data segment size:           %ju bytes\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_FSIZE, &limits);
    printf("Soft limit for size of files, created by process:   %ju bytes\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for size of files, created by process:   %ju bytes\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_RTPRIO, &limits);
    printf("Soft limit for real time priority:                  %ju\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for real time priority:                  %ju\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_LOCKS, &limits);
    printf("Soft limit for total number of blocks:              %ju\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for total number of blocks:              %ju\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_NOFILE, &limits);
    printf("Soft limit for number of fd's, opened by a process: %ju\n", (uintmax_t) limits.rlim_cur - 1);
    printf("Hard limit for number of fd's, opened by a process: %ju\n", (uintmax_t) limits.rlim_max - 1);
    getrlimit(RLIMIT_RTTIME, &limits);
    printf("Soft limit for total time of execution:             %ju microseconds\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for total time of execution:             %ju microseconds\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_SIGPENDING, &limits);
    printf("Soft limit for total number of signals:             %ju\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for total number of signals:             %ju\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_NPROC, &limits);
    printf("Soft limit for N of processes, created by this one: %ju\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for N of processes, created by this one: %ju\n", (uintmax_t) limits.rlim_max);
    getrlimit(RLIMIT_STACK, &limits);
    printf("Soft limit for total size of process' stack:        %ju bytes\n", (uintmax_t) limits.rlim_cur);
    printf("Hard limit for total size of process' stack:        %ju bytes\n", (uintmax_t) limits.rlim_max);

    printf("\nPriorities:\n");
    printf("The priority of a calling process:             %d\n", getpriority(PRIO_PROCESS, 0));
    printf("The priority of a group of a calling process:  %d\n", getpriority(PRIO_PGRP, 0));
    printf("The priority of a user of a calling process:   %d\n", getpriority(PRIO_USER, 0));

    struct sched_attr attributes;
    if (syscall(SYS_sched_getattr, 0, &attributes, sizeof(attributes), 0) < 0)
        perror("sched_getattr");
    else {
        printf("\nAlgorithm of planning: ");
        switch (attributes.sched_policy) {
            case (0):
                printf("SCHED_OTHER\n");
                break;
            case (1):
                printf("SCHED_FIFO\n");
                break;
            case (2):
                printf("SCHED_RR\n");
                break;
            case (3):
                printf("SCHED_BATCH\n");
                break;
            case (5):
                printf("SCHED_IDLE\n");
                break;
            case (6):
                printf("SCHED_DEADLINE\n");
                break;
        }
        if ((attributes.sched_policy == 0) || (attributes.sched_policy == 3))
            printf("Compliance: %d\n", attributes.sched_nice);
        if ((attributes.sched_policy == 1) || (attributes.sched_policy == 2))
            printf("Static priority: %u\n", attributes.sched_priority);
        if (attributes.sched_policy == 6) {
            printf("Runtime:  %ju\n", attributes.sched_runtime);
            printf("Deadline: %ju\n", attributes.sched_deadline);
            printf("Period:   %ju\n", attributes.sched_period);
        }
    }
    return;
}

int main(void) {
    proc_info();
    return 0;
}
