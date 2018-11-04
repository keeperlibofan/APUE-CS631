#include "apue/apue.h"

int main(int argc, char *argv[]) {
    int         i;
    char        **ptr;
    extern char **environ; // 这样这个变量可以引用 exec 函数传给他的environ函数

    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    for (ptr = environ; *ptr != 0/*解引出来的是一个字符串指针，如果它指向0地址*/; ptr++) {
        printf("%s\n", *ptr);
    }
    exit(0);
}