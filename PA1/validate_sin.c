#include <stdio.h>
#include "sin_helpers.c"

int main(int argc, char **argv) {
	// check for correct # of parameters
	if (argc != 2) {
        return 1;
    }

    int array[9];
    int check[9];

    populate_array(123456789, arr);
    populate_array(121212121, check);
    return 0;

}