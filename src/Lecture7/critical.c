#include <setjmp.h>
#include "../../apue/apue.h"

static void	sig_quit(int);

int
main(void)
{
    printf("pid: %d\n", getpid());
    sigset_t	newmask, oldmask, pendmask;

    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        err_sys("can't catch SIGQUIT");

    /*
     * Block SIGQUIT and save current signal mask.
     */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) // 保存了老的屏蔽字
        err_sys("SIG_BLOCK error");
    sleep(10);	/* SIGQUIT here will remain pending */

    if (sigpending(&pendmask) < 0) // 获取未决的信号
        err_sys("sigpending error");
    if (sigismember(&pendmask, SIGQUIT)) {
        printf("\nSIGQUIT pending\n");
    } else {
        printf("\nSIGQUIT not pending\n");
    }

    /*
     * Restore signal mask which unblocks SIGQUIT.
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)   // 检测是否有未决信号，如果有未决信号，就先让未决信号执行完已经注册号的处理函数
        err_sys("SIG_SETMASK error");

    printf("SIGQUIT unblocked\n");
    sleep(5);	/* SIGQUIT here will terminate with core file */
    exit(0);


}

static void
sig_quit(int signo)
{
    printf("caught SIGQUIT\n");
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) // 处理方式设置为默认动作
        err_sys("can't reset SIGQUIT");
}

/*
 *
pid: 81719
^\
SIGQUIT pending
caught SIGQUIT
SIGQUIT unblocked
^\fish: './a.out' terminated by signal SIGQUIT (Quit request from job control with core dump (^\))
⏎
 *
 * */