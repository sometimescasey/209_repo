#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include  <unistd.h>

// global
int GLOBAL_INT = 0;

void test() {
	char *string = "Hello";
	
	
	pid_t id = fork();
	// printf("Does this happen again? %s\n", string);
	
	if (id == -1) exit(-1); // fork failed
	if (id > 0) {
		printf("This is parent, id is: %d\n", id);
		exit(0);
	} else {
		// this is the child, id == 0
		printf("This is child, id is: %d\n", id);
		string = "Goodbye";
		printf("Calling test again: \n");
		while (GLOBAL_INT < 2) {
			GLOBAL_INT += 1;
			test();
		}
		exit(0);
	}
	printf("Does this happen again? %s\n", string);
}

int main() {
	test();
	return 0;
}