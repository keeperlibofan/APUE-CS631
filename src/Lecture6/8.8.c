#include "apue/apue.h"

int
main(void) {

    pid_t pid;
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0){ // first child
        if ((pid = fork()) < 0) {
            err_sys("fork error");
        } else if (pid > 0) { // first child
            exit(0);
        }
        sleep(2);
        printf("my ppid is %d", getppid());
        exit(0);
    }

    if (waitpid(pid, NULL, 0) != pid) {
        err_sys("waitpid error");

    }
}
