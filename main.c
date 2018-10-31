#include <sys/resource.h>

int
main() {
    struct rlimit *rlptr;
    getrlimit();
}
