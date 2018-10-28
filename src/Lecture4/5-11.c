#include "apue/apue.h"

void pr_stdio(const char *, FILE *);
int is_unbuffered(FILE *);
int is_linebuffered(FILE *);
int buffer_size(FILE *);

int main() {
    pr_stdio();
}

void pr_stdio(const char * name, FILE * fp) {

}

#if defined(_IO_UNBUFFERED)

#elif defined(__SNBF)

int buffer_size(FILE * fp) {
    return fp->_bf._size;
}

#elif defined(_IONBF)

#ifdef __LP64
#define _flag __pad[4]
#define _ptr __pad[1]
#define _base __pad[2]
#endif


int
is_unbuffered(FILE *fp) {
    return(fp->_flags & _IONBF);
}

int
is_linebuffered(FILE *fp) {
    return(fp->_flags & _IOLBF);
}

int
buffer_size(FILE *fp) {
#ifdef __LP64
    return (fp->);
#endif
}

#endif
