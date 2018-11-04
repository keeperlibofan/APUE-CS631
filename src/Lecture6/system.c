// 一个简单的system函数实现

#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

int system(const char *cmdstring) {
    pid_t pid;
    int status;


    if ((pid = fork()) < 0) {
        status = -1;
    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0); // -c 参数告诉shell程序取下一个命令行参数(这里是cmdstring)，而不是从标准输入或者从某一个文件中读命令
        _exit(127); // 放置任一标准I/O缓冲(这些缓冲会在fork中由父进程复制到子进程)在子进程中被冲洗.
    } else {            // father
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;  // 除开errno之外的错误都会被设置成为-1
                break;
            }
        }
    }
    return (status);
}
