#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// simple Linked List for handling the element list, and tracking factors
typedef struct Number Number;

struct Number {
	int value;
	Number *next;
	Number *prev;
};

Number* new_Number(int n) {
	Number * pointer = malloc(sizeof(Number));
	pointer->value = n;
	pointer->next = NULL;
	pointer->prev = NULL;
	return pointer;
}

Number* init_list(int n) {
	// fill in the linked list from 2 to n. O(n)
	// return pointer to the head
	Number *head = new_Number(2);
	Number *prev = head;

	for (int i = 3; i <= n; i++) {
		Number *current = new_Number(i);
		current->prev = prev; // update prev pointer of current
		prev->next = current; // update next pointer of prev
		prev = current; // set prev to current, for the next item
	}

	return head;
}

void walk_list(Number *head) {
	// walk the entire LL starting from head
	// stop when the item does not have a next
	Number *current = head;
	while (current) {
		printf("%d\n", current->value);
		current = current->next;
	}
	// printf("%d\n", current->value); // get that last value too
}

void walk_back(Number *head) {
	// walk the list backwards
	// for when used as LIFO stack
	Number *current = head;
	while (current) {
		printf("%d\n", current->value);
		current = current->prev;
	}
	// printf("%d\n", current->value); // get that last value too
}

void free_list(Number *head) {
	//cleanup all but head
	Number *current = head;
	Number *next;
	while (current) {
		next = current->next;
		free(current);
		current = next;
	}
}

Number* filter_list(Number *head, int m) {
	// walk the entire LL starting from head
	// remove items that are multiples of m
	// stop when the item does not have a next
	// return pointer to new head

	Number *new_head = head;

	Number *current = head;
	Number * prev;
	Number * next;

	int cur_val;
	
	while (current) {
		cur_val = current->value;
		// printf("%d\n", cur_val);
		if ((cur_val % m) == 0) {
			// printf(" %d is a multiple of %d! remove it\n", cur_val, m);
			// is a multiple; remove it
			if (current->prev && current->next) {
				// we're not removing head or tail. fix all links
				prev = current->prev;
				next = current->next;
				prev->next = next;
				next->prev = prev;
				free(current);
			} else if (current->next){
				// we're removing the head, return pointer to the next item (new head)
				// printf("remove head!\n");
				next = current->next;
				next->prev = NULL;
				new_head = next;
				free(current);
			} else {
				// we're removing the tail and we're done
				// printf("remove tail!\n");
				prev = current->prev;
				prev->next = NULL;
				free(current);
				break;
			}
		}
		current = current->next;
	}
	// done: return new_head, which may or may not have changed
	return new_head;
}

int next_m(Number *head) {
	// return the next m for the filter.
	return head->value; 
}

Number* push(Number *head, int n) {
	// push item to the linked list at head
	// return pointer to new head
	Number *current = new_Number(n); 
	// todo: this malloc will need to be cleaned up later
	if (head == NULL) {
		// printf("pushed to a null head\n");
		// LL is empty, just return the pointer to current
		return current;
	} else {
		head->next = current;
		current->prev = head;
		return current;
	}	
}

Number* factor_check(Number *factor_tracker, int *count, int m, int n) {
	printf("Checking %d mod %d: %d\n", n, m, n%m);
	if ((n % m) == 0) {
		*count += 1;
		printf("pushing m! = %d\n", m);
		Number *new_tracker_head = push(factor_tracker, m);
		return new_tracker_head;
	}
	return factor_tracker;
}

int isInList(int n, Number *head) { // O(length of list)
	// check if an integer appears in ll
	Number *current = head;
	while (current) {
		if (current->value == n) {
			return 1;
		}
		else {
			current = current->prev;
		}
	}
	return 0;
}

// int classifyNumber() {
// 	// gets sent back by the child process.
// 	// wait for this result and evaluate it

// 	// return 0 if prime

// 	// return 1 if product of 2 primes

// 	// return 2 if not prime, and not product of 2 primes
// }

int main(int argc, char **argv) {
	// argument check
	if (argc != 2 || strtol(argv[1], NULL, 10) < 1) {
		fprintf(stderr, "Usage: \n\tpfact n\n");
		exit(1);
	}

	int n = strtol(argv[1], NULL, 10);

	// testing LL
	Number *head = init_list(n);
	// walk_list(head);

	Number *factor_tracker = NULL; // initially, no factors
	int factor_count = 0;
	int old_fc;

	Number *m_tracker = NULL;
	int m = next_m(head);
	int m_count = 0;

	// try m until m >= sqrt(n)
	printf("sqrt(n) is: %lf\n", sqrt((double) n));
	while ((double) m <= sqrt((double) n)) {
		if ((double) m == sqrt((double) n)) {
			// special case: n is perfect square
			printf("special case: n is perfect square of two primes! %d, %d\n", m, m);
			exit(0);
		}
		old_fc = factor_count;
		m_count += 1;

		factor_tracker = factor_check(factor_tracker, &factor_count, m, n);
		if (factor_count > old_fc) {
			// factor was added; n must have been a multiple of m. Do condition checks
			if (factor_count == 2) {
				double a = factor_tracker->value;
				double b = factor_tracker->prev->value;
				double sqrtn = sqrt((double) n);

				printf("Not product of 2 primes. Found 2 prime factors both smaller than sqrt(n): %d and %d\n", (int) a, (int) b);
				exit(0);

			}
		}
		m_tracker = push(m_tracker, m);
		printf("------------- filtering m = %d\n", m);
		head = filter_list(head, m);
		m = next_m(head);
	}

	// now that we've hit but not tried the filter that's >= sqrt(n), we examine cases.
	if (factor_count == 0) {
		printf("No factors: number is prime\n");
		exit(0);
	} else if (factor_count == 1) {
		// we have exactly one factor: divide n by it and check if that is prime (is it in the list of remaining #s)
		int factor = factor_tracker->value;
		int q = n / factor;
		if (isInList(q, head)) {
			printf("%d has two prime factors: %d, %d\n", n, factor, q);
			exit(0);
		} else {
			printf("Did division check: %d is not product of two primes. (%d, %d)\n", n, factor, q);
			exit(0);
		}


	}


	printf("----------m's tried: \n");
	walk_back(m_tracker);
	printf("-----------remaining items: \n");
	walk_list(head);
	printf("-------- recorded prime factors: (%d counted)\n", factor_count);
	walk_back(factor_tracker);
	printf("----------\n");

	free_list(head);
	// free_list(m_tracker);
	free_list(factor_tracker);


	// set up pipe
	// make pipe and file descriptors
	// fork so both processes get a copy
	// redirect on the parent, close the right fds
	// redirect on the child, close the right fds
	// do things

	int fd[2]; // file descriptors
	int r; // store fork() return value

	if (pipe(fd) == -1) {
		// pipe failed
		perror("pipe");
		exit(1);
	}

	r = fork();

	if (r > 0) {
		// is parent
		// write to fd(1)
		// close all the other stuff

		// never reads from pipe; close fd[0]
    	if (close(fd[0]) == -1) {
      		perror("close");
    	}

    	// write 1 to pipe
    	int one = 1;
    	int two = 2;
    	int three = 3;
    	int four = 4;

    	int write_val = write(fd[1], &one, sizeof(int));
    	write(fd[1], &two, sizeof(int));
    	write(fd[1], &three, sizeof(int));
    	write(fd[1], &four, sizeof(int));

    	printf("write_val = %d\n", write_val);
    	if (write_val < 0) {
    		// write failed
    		perror("write");
    		exit(1);
    	}

	} else if (r == 0) {
		// is child
		// reads from fd[0]
		// close the other stuff

		// never writes to pipe; close fd[1]
    	if (close(fd[1]) == -1) {
      		perror("close");
    	}

    	int readbuf;
    	int read_val = read(fd[0], &readbuf, sizeof(readbuf));
    	printf("read_val = %d\n", read_val);
    	if (read_val > 0) {
    		printf("readbuf contains: %d\n", readbuf);
    	}

    	read_val = read(fd[0], &readbuf, sizeof(readbuf));
    	printf("read_val = %d\n", read_val);
    	if (read_val > 0) {
    		printf("readbuf contains: %d\n", readbuf);
    	}

    	read_val = read(fd[0], &readbuf, sizeof(readbuf));
    	printf("read_val = %d\n", read_val);
    	if (read_val > 0) {
    		printf("readbuf contains: %d\n", readbuf);
    	}

    	read_val = read(fd[0], &readbuf, sizeof(readbuf));
    	printf("read_val = %d\n", read_val);
    	if (read_val > 0) {
    		printf("readbuf contains: %d\n", readbuf);
    	}
	}

	return 0;
}