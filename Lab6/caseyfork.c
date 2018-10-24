#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void anotherProcess() {
        int ret;

    printf("%d | (parent) %d: Hi there\n",  getpid(), getppid());

    ret = fork();

    printf("%d | (parent) %d: Runs for both\n",  getpid(), getppid());

    if (ret < 0) {
        perror("error");
        exit(1);
    } else if (ret == 0) {
        // child
        printf("%d | (parent) %d: I am child\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child1\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child2\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child3\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child4\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child5\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child6\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am child7\n",  getpid(), getppid());
    } else {
        // parent
        printf("%d | (parent) %d: I am parent\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent1\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent2\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent3\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent4\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent5\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent6\n",  getpid(), getppid());
        printf("%d | (parent) %d: I am parent7\n",  getpid(), getppid());
    }

    printf("%d | (parent) %d: This is outside the if block\n",  getpid(), getppid());
}

int main() {
    anotherProcess();
    return 0;
}