// wait_demo.c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child exiting. PID=%d\n", getpid());
        return 0;
    } else {
        wait(NULL);
        printf("Parent collected child\n");
        sleep(20);
    }
}
