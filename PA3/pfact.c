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
		exit(1);
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

	int fd[2]; // file descriptors

	if (pipe(fd) == -1) {
			// pipe failed
			perror("pipe");
			exit(1);
		}

	// idea
	// if another filter is warranted, we fork() to make a child
	// we open a pipe: parent only writes, child only reads
	// we write ints only
	// to it we pass the length of the current element array, so child knows how many to read
	// we then pass a digit 0 or 1 indicating the number of factors found so far

	// if 1, the next digit is the found factor.

	// we never need 2 because once we have two, we exit


	// ---------------------

	// write order: ------------------------
	// number of factors found
	// then factors if any
	// then m

	// THEN IF FILTERING: ------------------------
	// then len of current array
	// then array

	
	while(1) {
		m = array[0]; // whatever is at the head of the array

		// if the next filter would be greater than sqrt(n), stop
	    if ((double) m > sqrt((double) n)) {
	    	exit(0);
	    }

		// check if n is multiple of m
	    printf("ppid: %d | pid: %d | ", getppid(), getpid());
	    printf("Check if n = %d is multiple of m = %d\n", n, m);
	    if ((n % m) == 0) { 
	    	// special case: is n = m^2?
	    	if (n == m*m) {
	    		printf("special perfect square case\n");
	    		printf("%d %d %d\n", n, m, m);
	    		exit(0);
	    	}

	    	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	    	printf("Adding to factors list: n = %d is multiple of m = %d\n", n, m);
	    	found_count += 1;
	    	factors[found_count-1] = m;
	    	// add a factor to factors list
	    }
	    	
	    // if we have two factors now, exit
	    if (found_count == 2) {
			// child should exit and return 0
	    	printf("Case: found 2 factors\n");
	    	printf("%d is not the product of 2 primes\n", n);
	    	exit(0);
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

	    	// // write found_count and factors[i] if needed
	    	// int write_val = write(fd[1], &found_count, sizeof(int));
	    	// if (write_val > 0) {
	    	// 	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	    	// 	printf("wrote found_count value to pipe: %d\n", found_count);
	    	// }

	    	// for (int i = 0; i < found_count; i++) {
	    	// 	write(fd[1], &factors[i], sizeof(int));
	    	// 	printf("wrote factors[i] value to pipe: %d\n", factors[i]);
	    	// }

	    	// // write m
	    	// write_val = write(fd[1], &m, sizeof(int));
	    	// if (write_val > 0) {
	    	// 	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	    	// 	printf("wrote m value to pipe: %d\n", m);
	    	// }

	    	// // write array_len
	    	// write_val = write(fd[1], &array_len, sizeof(int));
	    	// if (write_val > 0) {
	    	// 	printf("ppid: %d | pid: %d | ", getppid(), getpid());
	    	// 	printf("wrote array_len to pipe: %d\n", array_len);
	    	// }

	    	// // write all values of the current array
	    	// for (int i = 0; i < array_len; i++) {
	    	// 	write_val = write(fd[1], &array[i], sizeof(int));
	    	// 	if (write_val > 0) {
	    	// 		printf("ppid: %d | pid: %d | ", getppid(), getpid());
	    	// 		printf("wrote value to pipe: %d\n", array[i]);
	    	// 	}
	    	// }

		} else if (r == 0) {
			// is child

			// // close write

			// if (close(fd[1]) == -1) {
	  //     		perror("close");
	  //   	} 

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
	    	for (int i = 0; i < array_len; i++) {
	    		int j = 0;
	    		if (array[i] != 0) {
	    			newarray[j] = array[i];
	    			j++;
	    		}
	    	}
	    	
	    	// free array* and replace it, and update len
	    	int *temp = array;
	    	free(temp);
	    	array = newarray; 
	    	array_len = filtered_len; 

	    	int r = fork();
		} else {
			// problem
			perror("fork");
			exit(1);
		}

	wait(NULL);
	}

}