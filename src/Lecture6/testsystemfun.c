#include "../../apue/apue.h"
#include <sys/wait.h>

int
main(void) {
    int status;

    if ((status = system("date")) < 0) {
        err_sys("system() error");
    }
    pr_exit(status); // 打印错误信息

    if ((status = system("nosuchcommand")) < 0) { // 状态为 127，无此条命令
        err_sys("system() error");
    }
    pr_exit(status); // 打印错误信息

    if ((status = system("who; exit 44")) < 0) { // 正常退出，状态为44
        err_sys("system() error");
    }
    pr_exit(status);

    exit(EXIT_SUCCESS);
}
