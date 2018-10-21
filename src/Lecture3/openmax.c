#include <errno.h>
#include <zconf.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>

#define OPEN_MAX_GUESS 256

int
open_max(void) {
#ifdef OPEN_MAX
    static int openmax = OPEN_MAX;
#else
    static int openmax = 0;
#endif
    if (openmax == 0) {
        // 先重新设置一下全局错误
        errno = 0;
        if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
            if (errno == 0) { // 如果在sysconf过程中errno没有被改变则sysconf运行正常
                openmax = OPEN_MAX_GUESS;
            } else {
                fprintf(stderr, "Sysconf error for _SC_OPEN_MAX");
            }
        }
    }
    return openmax;
}

//int main() {
//    struct rlimit rlp;
//
//    if (getrlimit(RLIMIT_NOFILE, &rlp) != 0) {
//        fprintf(stderr, "Unable to get per process rlimit: %s\n", strerror(errno));
//        exit(EXIT_FAILURE);
//    };
//
//    printf("This user could open %d file descriptors.\n",open_max());
//    printf("This process could open %d file descriptors.\n", (int)rlp.rlim_cur);
//    return EXIT_SUCCESS;
//}