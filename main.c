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

#define BUFFSIZE 4096
#ifndef MAXPATHLEN
#define MATHPATHLEN 1024
#endif

int main(int argc, char* argv[]) {
    int fd_from, fd_to, copy_bytes;
    char to_path[MATHPATHLEN] = ""; // 完整路径
    char *p_path;               // 前置路径
    char *slash = "/";
    char buf[BUFFSIZE];
    char *ptr;
    int byte_read, byte_write;

    // 参数校验
    if (argc != 3) {
        (void)fprintf(stderr ,"Usage: %s [file1] [file2]\n\
                or: %s [file1] [dir]\n\
                or: %s [file1] [...dir/file2]\n",
                       argv[0],argv[0],argv[0]);
        exit(EXIT_FAILURE);
    }

    // 打开源文件 source
    if ((fd_from = open(argv[1], O_RDONLY)) == -1) {
        (void)fprintf(stderr, "Open %s error", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*
     * 创造目标文件,超级用户才可以打开
     * 判断一下,如果是个路径，就构造成路径跟着文件名
     *
     * 需要使用到的函数：
     * strncpy, strncat
     *
     * 复制文件
     * while循环来复制文件
     */
    if ((fd_to = open(argv[2], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR)) == -1 ) {

        // check failed check if directory
        if (errno == EISDIR) {
            if ((int)strlen(argv[2] + (int)strlen(argv[1])) > MATHPATHLEN) {
                (void)fprintf(stderr, "The path length is too long.");
                exit(EXIT_FAILURE);
            }

            copy_bytes = (int)strlen(argv[2]);
            p_path = strncpy(to_path, argv[2], copy_bytes);
            if (p_path[copy_bytes-1] != '/') {
                strncat(to_path, slash, (int)strlen(slash));
            }

            p_path = strncat(to_path, argv[1], (int)strlen(argv[1]));
            if ((fd_to = open(p_path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR)) == -1) {
                (void)fprintf(stderr, "Can't open file %s", to_path);
                exit(EXIT_FAILURE);
            }
        }
    }

    byte_read = (int)read(fd_from, buf, BUFFSIZE);
    // 读循环
    while (byte_read) {
        if (byte_read == -1) {
            (void)fprintf(stderr, "Can't read file: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // 没有读完全
        else if (byte_read > 0) {
            ptr = buf;
            byte_write = (int)write(fd_to, ptr, byte_read);
            // 写循环
            while (byte_write) {
                if (byte_write == -1) {
                    fprintf(stderr, "Can't write file %s", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                if (byte_write == byte_read) {
                    break;
                }
                else if (byte_write > 0) {
                    ptr += byte_write;
                    byte_read -= byte_write;
                }


                byte_write = (int)write(fd_to, ptr, byte_read);
            }
        }
        byte_read = (int)read(fd_from, buf, BUFFSIZE);
    }

    // 回头要关闭文件描述符号
    close(fd_from);
    close(fd_to);
    exit(EXIT_SUCCESS);
}
