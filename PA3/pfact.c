#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>

void writeData(int fd1, int found_count, int * factors, int m, int array_len, int * array) {
// write found_count and factors[i] if needed
	int write_val = write(fd1, &found_count, sizeof(int));
	if (write_val > 0) {
		// printf("ppid: %d | pid: %d | ", getppid(), getpid());
		// printf("wrote found_count value to pipe: %d\n", found_count);
	}

	for (int i = 0; i < found_count; i++) {
		write(fd1, &factors[i], sizeof(int));
		// printf("wrote factors[i] value to pipe: %d\n", factors[i]);
	}

// write m
	write_val = write(fd1, &m, sizeof(int));
	if (write_val > 0) {
		// printf("ppid: %d | pid: %d | ", getppid(), getpid());
		// printf("wrote m value to pipe: %d\n", m);
	}

// write array_len
	write_val = write(fd1, &array_len, sizeof(int));
	if (write_val > 0) {
		// printf("ppid: %d | pid: %d | ", getppid(), getpid());
		// printf("wrote array_len to pipe: %d\n", array_len);
	}

// write all values of the current array
	for (int i = 0; i < array_len; i++) {
		write_val = write(fd1, &array[i], sizeof(int));
		if (write_val > 0) {
			// printf("ppid: %d | pid: %d | ", getppid(), getpid());
			// printf("wrote value to pipe: %d\n", array[i]);
		}
	}
}

int main(int argc, char **argv) {
	int og_pid = getpid();

// argument check
	if (argc != 2 || strtol(argv[1], NULL, 10) < 1) {
		fprintf(stderr, "Usage: \n\tpfact n\n");
		exit(-1);
	} 
	
	int user_arg = strtol(argv[1], NULL, 10);
	double int_check1 = strtod(argv[1], NULL);
	double int_check2 = (double)(int)(int_check1);
	if ((int_check1 - int_check2) != 0.0) {
		// not an integer
		fprintf(stderr, "Usage: \n\tpfact n\n");
		exit(-1);
	} else if (user_arg == 1) {
		// special case 1
		printf("1 is not prime\n");
		printf("Number of filters = 0\n");
		return 0;
	}

	int n = strtol(argv[1], NULL, 10);

// intialize array from 2 to n
	int *array = malloc((n-1)*sizeof(int));
	for (int i = 2; i <= n; i++ ) {
		array[i-2] = i;
		// printf("%d\n", i);
	}

	int array_len = n-1;

	int found_count = 0;
	int factors[2]; // we will find at most 2 factors

	int m;

	int hasSpawn = 0;

	int fd[2]; // file descriptors

	if (pipe(fd) == -1) {
	// pipe failed
		perror("pipe");
		exit(-1);
	}

	while(hasSpawn != 1) {
	m = array[0]; // whatever is at the head of the array
	if ((double) m > sqrt((double) n)) {

	// do condition checks
	// if we have found exactly one factor so far:
		if (found_count == 1) {
			int other_factor = n / factors[0];
		// search remaining array for other_factor
			int both_prime = 0;
			for (int i = 0; i < array_len; i++) {
				if (array[i] == other_factor) {
					both_prime = 1;
				}
			}

			if (both_prime) {
				printf("%d %d %d\n", n, factors[0], other_factor);
				// return 0;
			} else {
				printf("%d is not the product of two primes\n", n);
				// return 0;
			}
			// return 0;
		} else if (found_count == 0) {
			printf("%d is prime\n", n);
			// return 0;
		}

		if (getpid() == og_pid) { 
			// no filters ever ran; special case
			printf("Number of filters = 0\n");
		}

		return 0;
	}

	// check if n is multiple of m
	if ((n % m) == 0) { 
	// special case: is n = m^2?
		if (n == m*m) {
			printf("%d %d %d\n", n, m, m);
			
			if (getpid() == og_pid) { 
			// no filters ever ran; special case
				printf("Number of filters = 0\n");
			}
			return 0;
		}

		found_count += 1;
		factors[found_count-1] = m;
	// add a factor to factors list
	}

	// if we have two factors now, exit
	if (found_count == 2) {
		printf("%d is not the product of two primes\n", n);
		return 0;
	}

	// otherwise we're in business

	int r; // store fork() return value
	r = fork();

	if (r > 0) {
	// is parent

	// close read
		if (close(fd[0]) == -1) {
			perror("close");
		}

		writeData(fd[1], found_count, factors, m, array_len, array);

	} else if (r == 0) {
		// is child

		int readbuf;

		int read_val = read(fd[0], &readbuf, sizeof(readbuf));
		found_count = readbuf;

		if (found_count > 0) {
		// read the found factors
			for (int i = 0; i < found_count; i++) {
				read_val = read(fd[0], &readbuf, sizeof(readbuf));
				factors[i] = readbuf;
			}
		}

		read_val = read(fd[0], &readbuf, sizeof(readbuf));
		m = readbuf;

		read_val = read(fd[0], &readbuf, sizeof(readbuf));

		if (read_val > 0) {
			array_len = readbuf;
		} else {
			fprintf(stderr, "Error: read returned %d\n", read_val);
		}

	// walk through input list and filter
		int filtered_len = array_len;
		for (int i = 0; i < array_len; i++) {
			read_val = read(fd[0], &readbuf, sizeof(readbuf));
			if (read_val > 0) {
				if ((array[i] % m) == 0) {
				// remove this
				array[i] = 0; // zero out for removal later
				filtered_len -= 1;
			}
		} else {
			fprintf(stderr, "Error: read returned %d\n", read_val);
		}
	}

	int *newarray = malloc(sizeof(int) * filtered_len);
	int j = 0;
	for (int i = 0; i < array_len; i++) {
		if (array[i] != 0) {
			newarray[j] = array[i];
			j++;
		}
	}

	int *temp = array;
	free(temp);
	array = newarray; 
	array_len = filtered_len;
	m = array[0];


	} else {
		perror("fork");
		exit(-1);

	}

	int filter_count = 0;

	int status;
	wait(&status);

	if (WIFEXITED(status)) {
		int returned = WEXITSTATUS(status);
		if (returned < 0) {
			fprintf(stderr, "pid %d received a negative return value\n", getpid());
			exit(-1);
		}
		if (getpid() == og_pid) {
			filter_count = returned+1;
			printf("Number of filters = %d\n", filter_count);
		}
		return returned+1;
	}

	}
}

