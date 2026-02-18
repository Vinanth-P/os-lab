#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child running\n");
        sleep(2);
    } else {
        wait(NULL);
        printf("Parent after child\n");
    }
}
