/*
 * part1.c: a copy of empty.c with exactly five extra system calls.
 * Each function below is a thin wrapper from unistd.h that makes
 * one system call and nothing else.
 */
#include <unistd.h>

int main(void)
{
    getpid();   /* 1 */
    getppid();  /* 2 */
    getuid();   /* 3 */
    getgid();   /* 4 */
    geteuid();  /* 5 */
    return 0;
}
