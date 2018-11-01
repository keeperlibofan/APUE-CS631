#include "apue/apue.h"

void pr_exit() {
    WTERMSIG();
    WNOHANG;

}