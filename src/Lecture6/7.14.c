#include <stdio.h>

FILE *
open_data(void) {
    FILE *fp;

    char data[BUFSIZ];

    if ((fp = fopen("datafile", "r")) == NULL) {
        return(NULL);
    }
    if (setvbuf(fp, data, _IOLBF, BUFSIZ) != 0) {
        return(NULL);
    }
    return(fp);
}
