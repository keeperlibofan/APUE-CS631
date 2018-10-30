#include <stdio.h>
#include <stdlib.h>
#include "apue/apue.h"

static void my_exit1(void);
static void my_exit2(void);

int main() {
    if (atexit(my_exit2) != 0) { // 注册第三个终止处理函数, 由于是反向的，所以要先注册exit2函数
        err_sys("can't register my_exit2");
    }

    if (atexit(my_exit1) != 0) { // 注册第二个终止处理函数
        err_sys("can't register my_exit1");
    }
    if (atexit(my_exit1) != 0) { // 重复注册一遍
        err_sys("can't register my_exit1");
    }
    printf("main is done\n");
    return (0);
}

static void
my_exit1(void) {
    printf("first exit handler\n");
}

static void
my_exit2(void) {
    printf("second exit handler\n");
}