#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFERSIZE 64

int rangehi;
int rangelo;
int guess;

static void sigHandler(int signo) {
        switch(signo) {
        	case SIGUSR1:
        		// tell parent too high; reset rangehi
        		rangehi = guess-1;
        		return;
        	case SIGUSR2:
        		// tell parent too low
        		rangelo = guess+1; 
        		return;
        	case SIGINT:
        		kill (getppid(), 9); // kill parent with signal 9
        		exit(0);
        }
}


int main() {

	int fd[2];
 	  // make pipe
  	if (pipe(fd) == -1) {
    	perror("pipe");
    	exit(1);
  	} 
 	fd[0]; //-> for using read end
 	fd[1]; //-> for using write end

	int pid = fork();

	if (pid == 0) {
		// is child
		// child will write once to parent to let it know that guessing can start
		// close read end of child pipe
		if (close(fd[0]) == -1) {
      		perror("close");
    	}

		// get parent's pid
		pid_t ppid = getppid();

		int valid_secret = 0;

		char *buf;

		while (valid_secret != 1) {
			int hi = 1023;
			int lo = 0;
			printf("Please give a secret number between 0 and 1023 inclusive:\n");
			buf = malloc(BUFFERSIZE * sizeof buf);
			fgets(buf, BUFFERSIZE, stdin);

			// parse to int
			int secret;
			secret = strtol(buf, NULL, 10);

			// value check
			if (secret >= hi && secret <= lo) {
				valid_secret == 1;
			}
		}
		
		// write one byte to the pipe to let parent know that it can start guessing
		int dummy = 1;
		write(fd[1], &dummy, sizeof(dummy));

		int toohi, toolo, cor;
		int valid;

		// do forever
		while(1){
			valid = 0;
			while (valid != 1) {
			printf("Is the guess hi, lo, or correct?:\n");
			memset(buf, '\0', sizeof(*buf) * BUFFERSIZE);
			fgets(buf, BUFFERSIZE, stdin);

			// validate
			toohi = (strncmp(buf, "hi", 2) == 0);
			toolo = (strncmp(buf, "lo", 2) == 0);
			cor = (strncmp(buf, "correct", strlen("correct")) == 0);

			if (toohi || toolo || cor) {
				valid = 1;
			} else {
				printf("Invalid, please enter 'hi', 'lo', or 'correct':\n");
			}
		}

		if (toohi) {
			// guess was too high
			// child sends SIGUSR1
			kill(ppid, SIGUSR1);
		} else if (toolo) {
			// guess was too low
			// child sends SIGUSR2
			kill(ppid, SIGUSR2);
		} else {
			// must be correct
			// child sends SIGINT
			kill(ppid, SIGINT);
		}

		// write to parent to let it know it can guess again
		write(fd[1], &dummy, sizeof(dummy));

		}



	} else if (pid > 0){
		// is parent
		// make a guess

		// wait for child to write ready to the pipe before guessing
		// parent never writes; close fd[1]
		if (close(fd[1]) == -1) {
      		perror("close");
    	}

		srand(time(NULL));

		rangehi = 1023;
		rangelo = 0;

		while(1) { // guess forever until sigHandler breaks us out
			// sighandler setup
			struct sigaction handleSignal_hi;
			struct sigaction handleSignal_lo;
			struct sigaction handleSignal_co;

	    	memset(&handleSignal_hi, 0, sizeof(handleSignal_hi));
	    	memset(&handleSignal_lo, 0, sizeof(handleSignal_lo));
	    	memset(&handleSignal_co, 0, sizeof(handleSignal_co));

	    	handleSignal_hi.sa_handler = sigHandler;
	    	handleSignal_lo.sa_handler = sigHandler;
	    	handleSignal_co.sa_handler = sigHandler;

	    	sigaction(SIGUSR1, &handleSignal_hi, NULL);
	    	sigaction(SIGUSR2, &handleSignal_lo, NULL);
			sigaction(SIGINT, &handleSignal_co, NULL);

			// wait until child has gotten input from the user
			char getReady[BUFFERSIZE];
			read(fd[0], getReady, 1); // will block on this until child writes
			//while (strncmp(getReady, "r", 1) != 0) {
				// read(fd[0], getReady, 1); // will block on this until child writes
			//}
	    	
	    	// printf("this ran! getReady = %s\n", getReady);

			int randomGuess = rand() % (rangehi + 1 - rangelo) + rangelo;
			guess = randomGuess;
			printf("parent's random guess is %d\n", randomGuess);
			int status;
			wait(&status); // wait until a new signal is receivec
		}

	} else {
		// problem
		perror("fork");
		exit(1);
	}
	return 0;
}