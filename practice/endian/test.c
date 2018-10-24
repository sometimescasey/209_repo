#include <signal.h>
#include <stdio.h>
#include <unistd.h>


int x = 5;

void handler(int sig) {
    x += 3;
    fprintf(stderr, "inside %d ", x);
}

int main() {
    fprintf(stderr, "start ");
    //                             POSITION A
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT,&act,NULL);


    //                             POSITION B
    x += 2;

    //
    sleep(100);                             //POSITION C
    fprintf(stderr, "outside %d", x);

    return 0;
}