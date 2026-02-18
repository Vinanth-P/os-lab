#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int a = 10;
    if ((fork()) == 0)
        a++;
    printf("%d\n", a);
}
//fork()==0->child
//fork()>0->parent
