#include <sys/types.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv) {

    DIR *dp;
    struct dirent *dirp;

    if (argc != 2) {
        fprintf(stderr, "usage: %s dir_name\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    if ((dp = opendir(argv[1])) == NULL ) {
        fprintf(stderr, "can't open '%s'\n", argv[1]);
        exit(EXIT_SUCCESS);
    }

    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name); // 目录项的名字
    }


    closedir(dp);
    return(0);
}