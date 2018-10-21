#include <stdio.h>
#include <zconf.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main() {
    // prints the value of STDIN FILENO, STDOUT FILENO, STDERR FILENO
    printf("STDIN_FILENO: %d", STDIN_FILENO);
    printf("STDIN_FILENO: %d", STDOUT_FILENO);
    printf("STDIN_FILENO: %d", STDERR_FILENO);
    // BLAHBLAH

    int fd1, fd2, fd3;
    FILE *f;
    printf("\nOpening /dev/zero...\n");
    if ((fd1 = open("/dev/zero", O_RDONLY)) < 0) {
        fprintf(stderr, "Unable to open /dev/zero: %s\n", strerror(errno));
    } else {
        fprintf(stdout, "fd1: %d\n", fd1);
    }

    if ((fd2 = open("/dev/zero", O_RDONLY)) < 0) {

    }
    printf("\nNow closing fd1, but keeping fd2 open..\n");
    (void)close(fd1);

    printf("\nOpening /dev/zero a third time...\n");

    printf("Now opening /dev/zero as a stream.\n");
    if ((f = fopen("/dev/zero", "r")) == NULL) {
        fprintf(stderr, "Unable to open /dev/zero: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    };
    printf("f :%d", fileno(f));
    (void)fclose(f);

    return EXIT_SUCCESS;
}