#include <zconf.h>
#include <fcntl.h>
#include "apue/apue.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        err_quit("usage: a.out <pathname>");
    }
    if (access(argv[1], R_OK) < 0) {

    }
    AT_SYMLINK_NOFOLLOW;
    S_ISUID;
    _POSIX_CHOWN_RESTRICTED;

}