#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	int og_pid = getpid();
	// argument check
	if (argc != 2 || strtol(argv[1], NULL, 10) < 1) {
		fprintf(stderr, "Usage: \n\tpfact n\n");
		exit(1);
	}

	int n = strtol(argv[1], NULL, 10);

	int stopCreating = 0;
	int i = 0;

	while (i < n && (!stopCreating)) {
		int r = fork();
		if (r > 0) {
			// is a parent
			int stopCreating = 1;
		}
		printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
		i++;
	}
	wait(NULL);
}