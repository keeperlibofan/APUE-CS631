#include "../../apue/apue.h"
#include <errno.h>

void
pr_mask(const char* str) {
    sigset_t oldset;
    int      errno_save;

    errno_save = errno;
    if (sigprocmask(0, NULL, &oldset) < 0) {    // 获取当前进程的信号屏蔽字
        err_ret("sigprocmask error");
    } else {
        printf("%s", str);

        if sigismember(&oldset, SIGINT) {
            printf(" SIGINT");
        }
        if sigismember(&oldset, SIGQUIT) {
            printf(" SIGQUIT");
        }
        if sigismember(&oldset, SIGALRM) {
            printf(" SIGALRM");
        }

        printf("\n");
    }

    errno = errno_save; // restore errno
}