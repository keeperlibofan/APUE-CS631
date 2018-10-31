//
// Created by seadream on 2018/11/1.
//
#include <setjmp.h>
#include <apue/apue.h>

static void f1(int, int, int, int);
static void f2(void);

static jmp_buf jmpbuffer;
static int globval;


int
main(void) {
    int          autoval;
    register int regival;
    volatile int volaval;    // 强制变量不能回滚
    static int   statval;    // 静态变量在函数外部是不可以被使用到的，只有在内部使用的时候才能使用，退出该函数后， 尽管该变量还继续存在，但不能使用它。

    globval = 1; autoval = 2; regival = 3;
}

static void
f1(int i, int j, int k, int l) {
    longjmp();
}
