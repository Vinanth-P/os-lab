// Base program (parent + child running forever)
// fork_demo.c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child
        while (1) {
            printf("Child: PID=%d, PPID=%d\n", getpid(), getppid());
            sleep(2);
        }
    } else {
        // Parent
        while (1) {
            printf("Parent: PID=%d, Child=%d\n", getpid(), pid);
            sleep(2);
        }
    }

    return 0;
}
