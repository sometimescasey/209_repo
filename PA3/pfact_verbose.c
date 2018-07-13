 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// simple Linked List to store the remaining elements
typedef struct ll_item ll_item;

struct ll_item {
	int value;
	ll_item *next;
	ll_item *prev;
};

ll_item* new_ll_item(int n) {
	ll_item * pointer = malloc(sizeof(ll_item));
	pointer->value = n;
	return pointer;
}

ll_item* init_ll(int n) {
	// fill in the linked list from 2 to n. O(n)
	// return pointer to the head
	ll_item *head = new_ll_item(2);
	ll_item *prev = head;

	for (int i = 3; i <= n; i++) {
		ll_item *current = new_ll_item(i);
		current->prev = prev; // update prev pointer of current
		prev->next = current; // update next pointer of prev
		prev = current; // set prev to current, for the next item
	}

	return head;
}

void walk_ll(ll_item *head) {
	// walk the entire LL starting from head
	// stop when the item does not have a next
	ll_item *current = head;
	while (current) {
		printf("%d\n", current->value);
		current = current->next;
	}
	// printf("%d\n", current->value); // get that last value too
}

ll_item* filter_ll(ll_item *head, int m, ll_item *factor_tracker) {
	// walk the entire LL starting from head
	// remove items that are multiples of m
	// stop when the item does not have a next
	// if n is a factor of m, add it to factor_tracker
	// return pointer to new head

	ll_item *new_head = head;

	ll_item *current = head;
	ll_item * prev;
	ll_item * next;
	
	while (current) {
		int cur_val = current->value;
		printf("%d\n", cur_val);
		if ((cur_val % m) == 0) {
			printf(" %d is a multiple of %d! remove it\n", cur_val, m);
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
				printf("remove head!\n");
				next = current->next;
				next->prev = NULL;
				new_head = next;
				free(current);
			} else {
				// we're removing the tail and we're done
				printf("remove tail!\n");
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

int next_m(ll_item *head) {
	// return the next m for the filter.
	return head->value; 
}

ll_item* push(ll_item *head, int n) {
	// push item to the linked list at head
	// return pointer to new head
	ll_item *current = new_ll_item(n); 
	// todo: this malloc will need to be cleaned up later
	head->prev = current;
	current->next = head;
	return current;
}

int main(int argc, char **argv) {
	// argument check
	if (argc != 2 || strtol(argv[1], NULL, 10) < 1) {
		fprintf(stderr, "Usage: \n\tpfact n\n");
		exit(1);
	}

	int n = strtol(argv[1], NULL, 10);


	// int current_filter = next_m(head);
	// next filter is the smallest integer in the array, or the array head

	// testing LL
	ll_item *head = init_ll(n);
	walk_ll(head);

	ll_item *factor_tracker;
	head = filter_ll(head, 2, factor_tracker);
	walk_ll(head);


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