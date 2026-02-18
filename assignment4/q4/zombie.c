// zombie_demo.c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child exiting. PID=%d\n", getpid());
        return 0;
    } else {
        printf("Parent sleeping. PID=%d\n", getpid());
        sleep(60);   // don't wait
    }
}
