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
		printf("ppid: %d | pid: %d | ", getppid(), getpid());
		printf("wrote found_count value to pipe: %d\n", found_count);
	}

	for (int i = 0; i < found_count; i++) {
		write(fd1, &factors[i], sizeof(int));
		printf("wrote factors[i] value to pipe: %d\n", factors[i]);
	}

// write m
	write_val = write(fd1, &m, sizeof(int));
	if (write_val > 0) {
		printf("ppid: %d | pid: %d | ", getppid(), getpid());
		printf("wrote m value to pipe: %d\n", m);
	}

// write array_len
	write_val = write(fd1, &array_len, sizeof(int));
	if (write_val > 0) {
		printf("ppid: %d | pid: %d | ", getppid(), getpid());
		printf("wrote array_len to pipe: %d\n", array_len);
	}

// write all values of the current array
	for (int i = 0; i < array_len; i++) {
		write_val = write(fd1, &array[i], sizeof(int));
		if (write_val > 0) {
			printf("ppid: %d | pid: %d | ", getppid(), getpid());
			printf("wrote value to pipe: %d\n", array[i]);
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

	int n = strtol(argv[1], NULL, 10);

// intialize array from 2 to n
	int *array = malloc((n-1)*sizeof(int));
	for (int i = 2; i <= n; i++ ) {
		array[i-2] = i;
		printf("%d\n", i);
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
printf("ppid: %d | pid: %d | ", getppid(), getpid());
printf("head of array is: %d\n", array[0]);

// if the next number is bigger than the current filter, we know the filter has been run


// if the next filter would be greater than sqrt(n), stop
if ((double) m > sqrt((double) n)) {
	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	printf("m > sqrt(n)! stop\n");

// do condition checks
// if we have found exactly one factor so far:
	if (found_count == 1) {
		printf("Have exactly one prime factor\n");
		int other_factor = n / factors[0];
	// search remaining array for other_factor
		int both_prime = 0;
		for (int i = 0; i < array_len; i++) {
			printf("Checking other_factor %d against array[i] %d \n", other_factor, array[i]);
			if (array[i] == other_factor) {
				both_prime = 1;
			}
		}

		if (both_prime) {
			printf("Did division check, both factors are prime\n");
			printf("%d %d %d\n", n, factors[0], other_factor);
			return 0;
		} else {
			printf("Did division check, other factor is not prime\n");
			printf("%d is not the product of two primes\n", n);
			return 0;
		}
	} else if (found_count == 0) {
		printf("%d is prime\n", n);
		return 0;
	}

	break;
}

// check if n is multiple of m
printf("ppid: %d | pid: %d | ", getppid(), getpid());
printf("Check if n = %d is multiple of m = %d\n", n, m);
if ((n % m) == 0) { 
// special case: is n = m^2?
	if (n == m*m) {
		printf("ppid: %d | pid: %d | ", getppid(), getpid());
		printf("special perfect square case\n");
		printf("%d %d %d\n", n, m, m);
		return 0;
	}

	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	printf("Adding to factors list: n = %d is multiple of m = %d\n", n, m);
	found_count += 1;
	factors[found_count-1] = m;
// add a factor to factors list
}

// if we have two factors now, exit
if (found_count == 2) {
	printf("Case: found 2 factors\n");
	printf("%d is not the product of 2 primes\n", n);
	return 0;
}

// otherwise we're in business

printf("About to make a fork(). May I? (type 1 if yes)\n");
int canmake = 0;
scanf("%d", &canmake);

int r; // store fork() return value

if (canmake == 1) {
	r = fork();
} else {
	printf("fork from pid %d canceled by user.\n", getpid());
	exit(-1);
}

if (r > 0) {
// is parent

// close read
	if (close(fd[0]) == -1) {
		perror("close");
	}

	writeData(fd[1], found_count, factors, m, array_len, array);

} else if (r == 0) {

	int readbuf;

	int read_val = read(fd[0], &readbuf, sizeof(readbuf));
	found_count = readbuf;
	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	printf("read found_count = %d\n", found_count);

	if (found_count > 0) {
	// read the found factors
		for (int i = 0; i < found_count; i++) {
			read_val = read(fd[0], &readbuf, sizeof(readbuf));
			factors[i] = readbuf;
			printf("read factor[i] = %d\n", factors[i]);
		}
	}

	read_val = read(fd[0], &readbuf, sizeof(readbuf));
	m = readbuf;
	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	printf("read m = %d\n", m);



	read_val = read(fd[0], &readbuf, sizeof(readbuf));
// printf("read_val = %d\n", read_val);

	if (read_val > 0) {
		array_len = readbuf;
		printf("ppid: %d | pid: %d | ", getppid(), getpid());
		printf("read: array length is: %d\n", array_len);
	} else {
		fprintf(stderr, "Error: read returned %d\n", read_val);
	}

// walk through input list and filter before writing to the next filter
	int filtered_len = array_len;
	for (int i = 0; i < array_len; i++) {
		read_val = read(fd[0], &readbuf, sizeof(readbuf));
		if (read_val > 0) {
			printf("ppid: %d | pid: %d | ", getppid(), getpid());
			printf("Reading array value: %d\n", readbuf);
			if ((array[i] % m) == 0) {
				printf("Remove: %d\n", array[i]);
			// remove this
			array[i] = 0; // zero out for removal later
			filtered_len -= 1;
		}
	} else {
		fprintf(stderr, "Error: read returned %d\n", read_val);
	}
}

int *newarray = malloc(sizeof(int) * filtered_len);
// collect the non-zero items from array
int j = 0;
for (int i = 0; i < array_len; i++) {
	if (array[i] != 0) {
		printf("newarray[%d] is now array[%d] = %d\n", j, i, array[i]);
		newarray[j] = array[i];
		j++;
	}
}

// free array* and replace it, and update len
int *temp = array;
free(temp);
array = newarray; 
array_len = filtered_len;
printf("array is now:\n");
for (int i = 0; i < array_len; i++) {
	printf("%d, ", array[i]);
}
printf("\n");
m = array[0];



//  

} else {
// problem
	perror("fork");
	exit(-1);

}

int status;
wait(&status);
if (WIFEXITED(status)) {
	int returned = WEXITSTATUS(status);
	if (returned < 0) {
		fprintf(stderr, "pid %d received a negative return value\n", getpid());
		exit(-1);
	}
	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	printf("exited normally with status %d\n", returned);
	printf("pid %d is returning %d\n", getpid(), returned+1);
	if (getpid() == og_pid) {
		int result = returned+1;
		printf("Number of filters = %d\n", result);
	}
	return returned+1;
}



}
}

