#include <pwd.h>
#include <stddef.h>
#include <string.h>

struct passwd *
getpwnam(const char *name) {
    struct passwd * ptr;

    setpwent(); // ??
    while ((ptr = getpwent()) != NULL) {
        if (strcmp(name, ptr->pw_name) == 0) {
            break;
        }
    }
    endpwent();
    return ptr;
}

struct passwd *
getpwuid(uid_t uid) {
    struct passwd *ptr;
    setpwent();
    while ((ptr = getpwent()) != NULL) {
        if (uid == ptr->pw_uid) {
            break;
        }
    }
    endpwent();
    return ptr;
}