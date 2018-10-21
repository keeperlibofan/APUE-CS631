#include <printf.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <zconf.h>

/*
 * This trivial program illustrates how to open files.  We present the
 * following use cases:
 *
 * - create a non-existent file
 * - "create" an existing file
 * - fail to create an existing file
 * - open an existing file
 * - fail to open a non-existing file
 * - truncate an existing file
 */


 void createFile() {
    int fd;

    printf("Trying to create './newfile' with O_RDONLY | O_CREAT...\n");

    if ((fd = open("./newfile", O_RDONLY | O_CREAT,
                   S_IRUSR | S_IWUSR)) == -1) {
        fprintf(stderr, "Unable to create './newfile': %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("'./newfile' created. File descriptor is: %d\n", fd);

    /* We are leaking a file descriptor here to illustrate that they
     * are increasing as we open more files.  Normally, we would call
     * close(2) whenever we're done using the decriptor. */
 }

void
failExclFileCreation() {
    int fd;
    printf("Trying to create './newfile' with O_RDONLY | O_CREAT | O_EXCL...\n");

    if ((fd = open("./newfile", O_RDONLY | O_CREAT | O_EXCL,
                S_IRUSR | S_IWUSR)) == -1) { // S_IRUSR
        fprintf(stderr, "Unable to create './newfile': %s\n", strerror(errno));
    }

 /* We expect this to fail! */
    if (close(fd) == -1) {
        fprintf(stderr, "Closing failed: %s\n", strerror(errno));
    }
}

void
failOpenNonexistingFile() {
    int fd;

    printf("Trying to open (non-existant) './nosuchfile' with O_RDONLY...\n");

    if ((fd = open("./nosuchfile", O_RDONLY)) == -1) {
        fprintf(stderr, "Unable to open './nosuchfile': %s\n",
                strerror(errno));
    }

    /* This will fail.  So be it. */
    (void)close(fd);
}

void
openFile() {
    int fd;

    printf("Trying to open './openex.c' with O_RDONLY...\n");

    if ((fd = open("./openex.c", O_RDONLY)) == -1) {
        fprintf(stderr, "Unable to open './openex.c': %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("'./openex.c' opened. File descriptor is: %d\n", fd);

    (void)close(fd);
    printf("'./openex.c' closed again\n");
}

void
truncateFile() {
    int fd;

    printf("Trying to open './newfile  with O_RDONLY | O_TRUNC...\n");

    if ((fd = open("./newfile", O_RDONLY | O_TRUNC)) == -1) {
        fprintf(stderr, "Unable to open './newfile': %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("'./newfile' opened. File descriptor is: %d\n", fd);
    printf("'./newfile' truncated -- see 'ls -l newfile'\n");

    (void)close(fd);
}


int
main() {
    createFile();
    system("ls -l newfile");
    printf("\n");
    sleep(2);

    createFile();
    system("ls -l newfile");
    printf("\n");
    sleep(2);

    failExclFileCreation();
    printf("\n");
    sleep(2);

    openFile();
    printf("\n");
    sleep(2);

    failOpenNonexistingFile();
    printf("\n");
    sleep(2);

    printf("cp openex.c newfile\n");
    system("cp openex.c newfile");
    system("ls -l newfile");
    truncateFile();
    system("ls -l newfile");

    return 0;
}