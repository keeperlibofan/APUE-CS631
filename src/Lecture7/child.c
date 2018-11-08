#include	"../../apue/apue.h"
#include	<sys/wait.h>

static void	sig_cld(int);
static void sig_cld2(int, siginfo_t* , void *);

int
main()
{
    pid_t	pid;
    int status;
    sigset_t sigset1;
    sigemptyset(&sigset1);

    struct sigaction sigaction1 = {
            sig_cld2,
            sigset1,
            SA_SIGINFO, // 代表这个结构中的第一个sig_cld是一个sa_sigaction
    };


    if (signal(SIGCHLD, sig_cld) == SIG_ERR) // Mac没有SIGCLD信号
        perror("signal error");

    if ((pid = fork()) < 0) {
        perror("fork error");
    } else if (pid == 0) {		/* child1 */
        sleep(2);
        _exit(0);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
    } else if (pid == 0) {      /* child2 */
        sleep(2);
        _exit(0);
    }
    pause();
    pid = wait(&status);	/* parent */
    printf("pid = %d\n", pid); // 第一次wait到了子进程1

    exit(0);
}


// 在处理程序中wait和重新注册信号处理函数
static void
sig_cld(int signo) /* 中断pause() */ {
    pid_t pid;
    int status;

    printf("SIGCLD received\n");

    //if (signal(SIGCHLD, sig_cld) == SIG_ERR)	/* reestablish handler */
    //    perror("signal error");

    if ((pid = wait(&status)) < 0)		/* fetch child status */
        perror("wait error");

    printf("pid = %d\n", pid);
}

static void
sig_cld2(int signo, siginfo_t * pSiginfo, void * pVoid) {

}

/*
在mac上的输出是：
╰─λ ./a.out                                                                                                                                                                          0 < 15:25:35
SIGCLD received
pid = 71831
⏎
 处理程序的确被调用到了，但是没有产生递归，所以子进程的退出型号只被捕捉到了一次

 */
