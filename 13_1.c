#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

void proc_info(const char *procname) { /* to get all information about the current process */
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

int main(void) {
    pid_t child_id = fork();
    if (child_id == -1) {          /* we failed to create a child process */
        perror("fork");
        return -1;
    }
    if (child_id == 0) {           /* this code is executed in the child process only */
        proc_info("child");
        if (kill(getpid(), SIGTERM) == -1) {
            perror("kill");
            return -1;
        }
    }
    else {                         /* this code is executed in the parent process only */
        proc_info("parent");
        int status;
        pid_t id = waitpid(-1, &status, WUNTRACED | WCONTINUED);
        if (id == -1) {
            perror("waitpid");
            return -1;
        }
        printf("PID of a child process, whose state changed: %d\n", id);
        if (WIFEXITED(status))
            printf("The child process terminated normally, with the exit status %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status)) 
            printf("The child process was killed by signal %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))
            printf("The child process was stopped by signal %d\n", WSTOPSIG(status));
        else if (WIFCONTINUED(status))
            printf("The child process continued\n");
    }
    return 0;
}
