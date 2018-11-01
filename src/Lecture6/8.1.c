#include "apue/apue.h"

int globvar = 6;
char buf[] = "a write to stdout\n";

int
main() {
    int var;    // 栈上的自动变量
    pid_t pid;

    var = 88;
    if (write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1) { // sizeof 求出来的字符串长度通常要大1
        err_sys("write error");
    }
    printf("before fork\n");
}

