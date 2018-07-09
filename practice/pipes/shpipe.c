#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	char *filename;
	if (argc != 2) {
		printf("Usage: shpipe file\n");
		exit(1);
	} else {
		filename = argv[1];
	}
	int fd[2]; // file descriptors for pipe
	int r; // fork

	// make pipe
	if (pipe(fd) == -1) {
		perror("pipe");
		exit(1);
	} // open pipe and put descriptors at fd[0] and fd[1]

	r = fork();
	if (r > 0) {
		// is parent
		// Open file
		int f_in = open(filename, O_RDONLY);
		if (f_in) {
			// reset STDIN slot to read from file
			if (dup2(f_in, fileno(stdin)) == -1) {
				perror("dup2");
				exit(1);
			}
			// reset STDOUT slot to write to pipe: aka fd[1]
			// pipe is read-write, 0-1
			if (dup2(fd[1], fileno(stdout)) == -1) {
				perror("dup2");
				exit(1);
			}
			// close unneeded file descriptors: piperead, fd[1], and the file
			if (close(fd[0]) == -1) {
				perror("close");
				// no need to terminate whole thing if this fails tho?
			}
			if (close(fd[1]) == -1) {
				perror("close");
			}
			if (close(f_in) == -1) {
				perror("close");
			}
		} else {
			perror("open");
			exit(1);
		}

		// now run sort
		// (char *) 0 is null pointer
		// execl("/usr/bin/sort", "sort", (char *) 0);
		execl("/usr/bin/sort", "sort", NULL);
		fprintf(stderr, "ERROR: exec should not return \n");
	}
	else if (r == 0) {
		// is child
		// reset stdin to read from pipe
		if (dup2(fd[0], fileno(stdin)) == -1) {
				perror("dup2");
				exit(1);
		}

		// never writes to pipe; close fd[1]
		if (close(fd[1]) == -1) {
			perror("close");
		}
		// now close fd[0] after redirecting
		if (close(fd[0]) == -1) {
			perror("close");
		}
		// we don't have a file here since f_in was only opened in parent

		execl("/usr/bin/uniq", "uniq", NULL);
		fprintf(stderr, "ERROR: exec should not return \n");
	}
	else {
		// error
		perror("fork");
		exit(1);
	}
	return 0;
}