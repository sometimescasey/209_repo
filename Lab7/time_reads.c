/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define write_to 100

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

static void sigHandler(int signo) {
        printf(MESSAGE, num_reads, seconds);
        exit(0);
}

/* The first command line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    srand(time(NULL));
    int r_i;
    int seek;
    int *buffer = malloc(sizeof(int));
    int result;

    struct sigaction handleSignal;
    memset(&handleSignal, 0, sizeof(handleSignal));
    handleSignal.sa_handler = sigHandler;
    sigaction(SIGPROF, &handleSignal, NULL);

    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;

    // NOTE: must initialize these even if we don't use them, or can get stick in infloop
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_PROF, &timer, NULL);

    num_reads = 0;
    for (;;) {
      r_i = rand() % write_to;

      seek = fseek(fp, r_i, SEEK_SET);
      if (seek == 0) {
        result = fread(buffer, 1, 1, fp);
        if (result == 1) {
          num_reads += 1;
          printf("%d\n", *buffer);  
        } else {
          perror("fread");
          exit(1);
        }    
      } else {
        perror("fseek");
        exit(1);
      }
    }

    return 1; // something is wrong if we ever get here!
}
