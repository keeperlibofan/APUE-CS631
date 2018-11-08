#include "../../apue/apue.h"

Sigfunc *
mySignal(int signo, Sigfunc *func) {
    struct sigaction act, oact;
    act.__sigaction_u.__sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) { // alarm clock interrupt
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;       // 中断的系统调用不进行自动重启，如果没有这个信号就是默认系统调用被中断了默认不重启动
#endif
    } else { // 其他中断，我们希望自动重启动
        act.sa_flags |= SA_RESTART;         // 中断的系统调用自动进行重启动
    }
    if (sigaction(signo, &act, &oact) < 0) {
        return (SIG_ERR);
    }
    return (oact.__sigaction_u.__sa_handler);
}

Sigfunc *
signal_intr(int signo, Sigfunc * func) {
    struct sigaction act, oact;
    act.__sigaction_u.__sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &act, &oact) < 0) {
        return (SIG_ERR);
    }
    return (oact.__sigaction_u.__sa_handler);
}

int main() {
    sigprocmask();
}