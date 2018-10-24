/*
 *   CS631 Advanced Programming in the UNIX Environment
 *                    Assignment 2
 *                  Author: Bofan Li
 *                  Date:Oct,21,2018
 */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <zconf.h>
#include <sys/stat.h>
#include "apue/apue.h"

int main(int argc, char* argv[]) {


}

int mydup2(int oldfd, int newfd) {
    struct stat s;

    // Check if old fd is a valid descriptor, fail with EBADF is it's not.
    if (fstat(oldfd, &s) < 0) {
        errno = EBADF; /* Bad file descriptor */
    };

    // Check newfd is in valid range, fail with EBADF otherwise.
    // Can return -1; that means either no limit, or error getting the limit.  Either case is fine here.
    int openmax = sysconf(_SC_OPEN_MAX);
    if (newfd < 0 || oldfd > openmax) {
        errno = EBADF;
        return -1;
    }

    // If oldfd == newfd just return newfd.
    if (oldfd == newfd) {
        return newfd;
    }
    // If newfd is open, close it.
    if (fstat(newfd, &s) == 0) {
        close(newfd);
    }

    int to_close_ptr = 0;
    int *to_close = malloc(sizeof(int) * newfd);

    while (1) {
        int current_try;
        // 如果相等
        current_try = dup(oldfd); // 给分配一个fd数字
        if (current_try == newfd) {
            for (int i = 0; i < newfd; i++) {
                close(i);
            }
            return newfd;
        } else {
            to_close[to_close_ptr++] = current_try;
        }
    }
}