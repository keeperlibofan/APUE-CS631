//
// Created by seadream on 2018/10/27.
//
#include <zconf.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include "apue/apue.h"
#include <sys/stat.h>

typedef int Myfunc(const char*, const struct stat*, int);

static Myfunc myfunc;
static int myftw(char*, Myfunc*);
static int dopath(Myfunc*);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char* argv[]) {

}

#define FTW_F 1;
#define FTW_D 2;
#define FTW_DNR 3;
#define FTW_NS 4;

static char *fullpath;
static size_t pathlen;

static int
myftw(char* pathname, Myfunc* func) {
    fullpath = path_alloc(&pathlen);

    if (pathlen <= strlen(fullpath)) {
        pathlen *= 2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL);
        err_sys("realloc failed"); // exit(1)
    }
    strcpy(fullpath, pathname);
    return(dopath(func));
}

// return whatever func return
static int
dopath(Myfunc* func) {
    struct stat  statbuf;
    struct dirent *dirp;
    DIR           *dp;
    int           ret, n;

    if ((lstat(fullpath, &statbuf)) < 0) {
        return(func(fullpath, &statbuf, FTW_NS));
    }
    if (S_ISDIR(statbuf.st_mode) == 0) { // 不是目录
        return func(fullpath, &statbuf, FTW_F);
    }

    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0) {
        //return(ret);
    }
    n = strlen(fullpath);
    if ((n + NAME_MAX + 2) > pathlen) {
        pathlen *= 2;
        if (fullpath = realloc(fullpath, pathlen) == NULL) {
            err_sys("realloc failed");
        }; // 原始数据会被复制，截取新的长度。
    }

}

static int
myfunc(const char* pathname, const struct stat* statptr, int type) {
    switch (type) {
        case FTW_F:
            switch (statptr->st_mode & S_IFMT) {
                case S_IFREG: nreg++;
                case S_IFBLK: nblk++;
                case S_IFCHR: nchr++;
                case S_IFIFO: nfifo++;
                case S_IFLNK: nslink++;
                case S_IFSOCK: nsock++;
                case S_IFDIR: // 目录应该FTW_D 的type
                    err_dump("for S_IFDIR for %s", pathname);
            }
            break;
        case FTW_D:
            ndir++;
            break;

    }
    return 0;
}
