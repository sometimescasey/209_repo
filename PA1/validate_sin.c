#include <stdio.h>
#include <stdlib.h>
#include "sin_helpers.c"

int main(int argc, char **argv) {
	// check for correct # of parameters
	if (argc != 2) {
        return 1;
    }

    int candidate = strtol(argv[1], NULL, 10);
    int array[9];
    int pop_array = populate_array(candidate, array);

    if (pop_array == 1) {
    	// fprintf(stderr, "Provided SIN was not 9 digits, excluding leading 0s\n");
    	return 1;
    }

    int check_result = check_sin(array);

    if (check_result == 0) {
    	printf("Valid SIN\n");
    } else {
    	printf("Invalid SIN\n");
    }

    return 0;

}