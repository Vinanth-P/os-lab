#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {  // child
        printf("Child executing ls...\n");
        execlp("ls", "ls", "-l", NULL);
        exit(0);     // runs only if exec fails
    } else {         // parent
        wait(NULL);
        printf("Child finished\n");
        exit(0);
    }
}

// fork() → create child

// exec() → replace child with another program

// wait() → parent waits for child

// exit() → terminate process